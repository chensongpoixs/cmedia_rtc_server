/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

//#define MS_CLASS "webrtc::TrendlineEstimator"
// #define MS_LOG_DEV_LEVEL 3

#include "modules/congestion_controller/goog_cc/trendline_estimator.h"

#include "modules/remote_bitrate_estimator/include/bwe_defines.h"
#include "rtc_base/numerics/safe_minmax.h"

//#include "Logger.hpp"

#include <absl/types/optional.h>
#include <math.h>
#include <algorithm>
#include <string>

#include "clog.h"

using namespace chen;

namespace webrtc {

namespace {

// Parameters for linear least squares fit of regression line to noisy data.
constexpr size_t kDefaultTrendlineWindowSize = 20;
constexpr double kDefaultTrendlineSmoothingCoeff = 0.6;
constexpr double kDefaultTrendlineThresholdGain = 4.0;
const char kBweWindowSizeInPacketsExperiment[] =
    "WebRTC-BweWindowSizeInPackets";

size_t ReadTrendlineFilterWindowSize(
    const WebRtcKeyValueConfig* key_value_config) {
  std::string experiment_string =
      key_value_config->Lookup(kBweWindowSizeInPacketsExperiment);
  size_t window_size;
  int parsed_values =
      sscanf(experiment_string.c_str(), "Enabled-%zu", &window_size);
  if (parsed_values == 1) {
    if (window_size > 1)
      return window_size;
    WARNING_EX_LOG("window size must be greater than 1");
  }
  WARNING_EX_LOG(
    "failed to parse parameters for BweWindowSizeInPackets"
    " experiment from field trial string, using default");
  return kDefaultTrendlineWindowSize;
}

absl::optional<double> LinearFitSlope(
    const std::deque<std::pair<double, double>>& points) {
  //RTC_DCHECK(points.size() >= 2);
  // Compute the "center of mass".
  double sum_x = 0;
  double sum_y = 0;
  for (const auto& point : points) {
    sum_x += point.first;
    sum_y += point.second;
  }
  double x_avg = sum_x / points.size();
  double y_avg = sum_y / points.size();
  // Compute the slope k = \sum (x_i-x_avg)(y_i-y_avg) / \sum (x_i-x_avg)^2
  double numerator = 0;
  double denominator = 0;
  for (const auto& point : points) {
    numerator += (point.first - x_avg) * (point.second - y_avg);
    denominator += (point.first - x_avg) * (point.first - x_avg);
  }
  if (denominator == 0)
    return absl::nullopt;
  return numerator / denominator;
}

constexpr double kMaxAdaptOffsetMs = 15.0;
constexpr double kOverUsingTimeThreshold = 30;
constexpr int kMinNumDeltas = 60;
constexpr int kDeltaCounterMax = 1000;

}  // namespace

TrendlineEstimator::TrendlineEstimator(
    const WebRtcKeyValueConfig* key_value_config,
    NetworkStatePredictor* network_state_predictor)
    : TrendlineEstimator(
          key_value_config->Lookup(kBweWindowSizeInPacketsExperiment)
                      .find("Enabled") == 0
              ? ReadTrendlineFilterWindowSize(key_value_config)
              : kDefaultTrendlineWindowSize,
          kDefaultTrendlineSmoothingCoeff,
          kDefaultTrendlineThresholdGain,
          network_state_predictor) {}

TrendlineEstimator::TrendlineEstimator(
    size_t window_size,
    double smoothing_coef,
    double threshold_gain,
    NetworkStatePredictor* network_state_predictor)
    : window_size_(window_size),
      smoothing_coef_(smoothing_coef),
      threshold_gain_(threshold_gain),
      num_of_deltas_(0),
      first_arrival_time_ms_(-1),
      accumulated_delay_(0),
      smoothed_delay_(0),
      delay_hist_(),
      k_up_(0.0087),
      k_down_(0.039),
      overusing_time_threshold_(kOverUsingTimeThreshold),
      threshold_(12.5),
      prev_modified_trend_(NAN),
      last_update_ms_(-1),
      prev_trend_(0.0),
      time_over_using_(-1),
      overuse_counter_(0),
      hypothesis_(BandwidthUsage::kBwNormal),
      hypothesis_predicted_(BandwidthUsage::kBwNormal),
      network_state_predictor_(network_state_predictor) {
  DEBUG_EX_LOG(
    "using Trendline filter for delay change estimation with window size: %zu",
    window_size_);
}

TrendlineEstimator::~TrendlineEstimator() {}

void TrendlineEstimator::Update(double recv_delta_ms,
                                double send_delta_ms,
                                int64_t send_time_ms,
                                int64_t arrival_time_ms,
                                bool calculated_deltas) {
  if (calculated_deltas) {
    const double delta_ms = recv_delta_ms - send_delta_ms;
    ++num_of_deltas_;
    num_of_deltas_ = std::min(num_of_deltas_, kDeltaCounterMax);
    if (first_arrival_time_ms_ == -1)
      first_arrival_time_ms_ = arrival_time_ms;

    // Exponential backoff filter.
    accumulated_delay_ += delta_ms;
    // BWE_TEST_LOGGING_PLOT(1, "accumulated_delay_ms", arrival_time_ms,
                          // accumulated_delay_);
    // smoothed_delay_ = smoothing_coef_ * smoothed_delay_ +
                      // (1 - smoothing_coef_) * accumulated_delay_;
    // MS_NOTE: Apply WEMA to the current delta_ms. Don't consider the
    // accumulated delay. Tests show it behaves more robustly upon delta peaks.
    smoothed_delay_ = smoothing_coef_ * delta_ms +
                      (1 - smoothing_coef_) * smoothed_delay_;
    // BWE_TEST_LOGGING_PLOT(1, "smoothed_delay_ms", arrival_time_ms,
                          // smoothed_delay_);

    // Simple linear regression.
    delay_hist_.push_back(std::make_pair(
        static_cast<double>(arrival_time_ms - first_arrival_time_ms_),
        smoothed_delay_));
    if (delay_hist_.size() > window_size_)
      delay_hist_.pop_front();
    double trend = prev_trend_;
    if (delay_hist_.size() == window_size_) {
      // Update trend_ if it is possible to fit a line to the data. The delay
      // trend can be seen as an estimate of (send_rate - capacity)/capacity.
      // 0 < trend < 1   ->  the delay increases, queues are filling up
      //   trend == 0    ->  the delay does not change
      //   trend < 0     ->  the delay decreases, queues are being emptied
      trend = LinearFitSlope(delay_hist_).value_or(trend);
    }

    // BWE_TEST_LOGGING_PLOT(1, "trendline_slope", arrival_time_ms, trend);

    DEBUG_EX_LOG("trend:%f, send_delta_ms:%f, recv_delta_ms:%f, delta_ms:%f arrival_time_ms:%lli, accumulated_delay_:%f, smoothed_delay_:%f", trend, send_delta_ms, recv_delta_ms, delta_ms, arrival_time_ms, accumulated_delay_, smoothed_delay_);
    Detect(trend, send_delta_ms, arrival_time_ms);
  }
  else {
	  DEBUG_EX_LOG("no calculated deltas");
  }

  if (network_state_predictor_) {
    hypothesis_predicted_ = network_state_predictor_->Update(
        send_time_ms, arrival_time_ms, hypothesis_);
  }
}

BandwidthUsage TrendlineEstimator::State() const {
  return network_state_predictor_ ? hypothesis_predicted_ : hypothesis_;
}

void TrendlineEstimator::Detect(double trend, double ts_delta, int64_t now_ms) {
  if (num_of_deltas_ < 2) {
    hypothesis_ = BandwidthUsage::kBwNormal;
    return;
  }
  const double modified_trend =
      std::min(num_of_deltas_, kMinNumDeltas) * trend * threshold_gain_;
  prev_modified_trend_ = modified_trend;
  // BWE_TEST_LOGGING_PLOT(1, "T", now_ms, modified_trend);
  // BWE_TEST_LOGGING_PLOT(1, "threshold", now_ms, threshold_);
  if (modified_trend > threshold_) {
    if (time_over_using_ == -1) {
      // Initialize the timer. Assume that we've been
      // over-using half of the time since the previous
      // sample.
      time_over_using_ = ts_delta / 2;
    } else {
      // Increment timer
      time_over_using_ += ts_delta;
    }
    overuse_counter_++;
    if (time_over_using_ > overusing_time_threshold_ && overuse_counter_ > 1) {
      if (trend >= prev_trend_) {
        time_over_using_ = 0;
        overuse_counter_ = 0;
		DEBUG_EX_LOG("hypothesis_: BandwidthUsage::kBwOverusing");

#if MS_LOG_DEV_LEVEL == 3
        for (auto& kv : delay_hist_) {
          MS_DEBUG_DEV("arrival_time_ms - first_arrival_time_ms_:%f, smoothed_delay_:%f", kv.first, kv.second);
        }
#endif

        hypothesis_ = BandwidthUsage::kBwOverusing;
      }
    }
  } else if (modified_trend < -threshold_) {
    time_over_using_ = -1;
    overuse_counter_ = 0;
    hypothesis_ = BandwidthUsage::kBwUnderusing;
	DEBUG_EX_LOG("---- BandwidthUsage::kBwUnderusing ---");
  } else {
    time_over_using_ = -1;
    overuse_counter_ = 0;
	DEBUG_EX_LOG("---- BandwidthUsage::kBwNormal ---");
    hypothesis_ = BandwidthUsage::kBwNormal;
  }
  prev_trend_ = trend;
  UpdateThreshold(modified_trend, now_ms);
}

void TrendlineEstimator::UpdateThreshold(double modified_trend,
                                         int64_t now_ms) {
  if (last_update_ms_ == -1)
    last_update_ms_ = now_ms;

  if (fabs(modified_trend) > threshold_ + kMaxAdaptOffsetMs) {
    // Avoid adapting the threshold to big latency spikes, caused e.g.,
    // by a sudden capacity drop.
    last_update_ms_ = now_ms;
    return;
  }

  const double k = fabs(modified_trend) < threshold_ ? k_down_ : k_up_;
  const int64_t kMaxTimeDeltaMs = 100;
  int64_t time_delta_ms = std::min(now_ms - last_update_ms_, kMaxTimeDeltaMs);
  threshold_ += k * (fabs(modified_trend) - threshold_) * time_delta_ms;
  threshold_ = rtc::SafeClamp(threshold_, 6.f, 600.f);
  last_update_ms_ = now_ms;
}

}  // namespace webrtc
