/***********************************************************************************************
created: 		2023-02-20

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/
#include "cvideo_rtp_depacketizer_av1.h"
#include "cbuffer_reader.h"
#include "cbyte_buffer.h"
#include <array>
#include "clog.h"
namespace chen {


	static const uint8 kObuSizePresentBit = 0b0'0000'010;

	bool ObuHasExtension(uint8_t obu_header) {
		return obu_header & 0b0'0000'100u;
	}

	bool ObuHasSize(uint8_t obu_header) {
		return obu_header & kObuSizePresentBit;
	}

	bool RtpStartsWithFragment(uint8_t aggregation_header) {
		return aggregation_header & 0b1000'0000u;
	}
	bool RtpEndsWithFragment(uint8_t aggregation_header) {
		return aggregation_header & 0b0100'0000u;
	}
	int RtpNumObus(uint8_t aggregation_header) {  // 0 for any number of obus.
		return (aggregation_header & 0b0011'0000u) >> 4;
	}
	int RtpStartsNewCodedVideoSequence(uint8_t aggregation_header) {
		return aggregation_header & 0b0000'1000u;
	}

	 
		// AV1 format:
	//
	// RTP payload syntax:
	//     0 1 2 3 4 5 6 7
	//    +-+-+-+-+-+-+-+-+
	//    |Z|Y| W |N|-|-|-| (REQUIRED)
	//    +=+=+=+=+=+=+=+=+ (REPEATED W-1 times, or any times if W = 0)
	//    |1|             |
	//    +-+ OBU fragment|
	//    |1|             | (REQUIRED, leb128 encoded)
	//    +-+    size     |
	//    |0|             |
	//    +-+-+-+-+-+-+-+-+
	//    |  OBU fragment |
	//    |     ...       |
	//    +=+=+=+=+=+=+=+=+
	//    |     ...       |
	//    +=+=+=+=+=+=+=+=+ if W > 0, last fragment MUST NOT have size field
	//    |  OBU fragment |
	//    |     ...       |
	//    +=+=+=+=+=+=+=+=+
	//
	//
	// OBU syntax:
	//     0 1 2 3 4 5 6 7
	//    +-+-+-+-+-+-+-+-+
	//    |0| type  |X|S|-| (REQUIRED)
	//    +-+-+-+-+-+-+-+-+
	// X: | TID |SID|-|-|-| (OPTIONAL)
	//    +-+-+-+-+-+-+-+-+
	//    |1|             |
	//    +-+ OBU payload |
	// S: |1|             | (OPTIONAL, variable length leb128 encoded)
	//    +-+    size     |
	//    |0|             |
	//    +-+-+-+-+-+-+-+-+
	//    |  OBU payload  |
	//    |     ...       |

		struct obu_payload
		{
			uint8*   start_address;
			size_t	 size;
		};
		class carray_of_array_views
		{
		public:
		public:
			class const_iterator;
			carray_of_array_views() = default;
			carray_of_array_views(const carray_of_array_views&) = default;
			carray_of_array_views& operator=(const carray_of_array_views&) = default;
			~carray_of_array_views() = default;

			bool alloc(size_t size)
			{
				data_ = static_cast<uint8*> (::malloc(sizeof(uint8)* size));
				if (!data_)
				{
					WARNING_EX_LOG("array of array view alloc [size = %u] failed !!!", size);
					return false;
				}
				size_ = 0;
				max_size_ = size;
				return true;
			}
			void free()
			{
				all_data.clear();
				if (data_)
				{
					::free(data_);
					data_ = NULL;
				}
				
			}

			const_iterator begin() const;
			const_iterator end() const;
			bool empty() const { return all_data.empty(); }
			size_t size() const { return size_; }
			bool is_init() const { return data_ != NULL; }
			void CopyTo(uint8_t* destination, const_iterator first) const;

			void Append(const uint8_t* data, size_t size)
			{
				//data_.emplace_back(data, size);
				memcpy(data_ + size_, data, size);
				obu_payload payload;
				payload.start_address = data_ + size_;
				payload.size = size;
				all_data.emplace_back(payload);
				size_ += size;
			}

		private:
			//using Storage = absl::InlinedVector<rtc::ArrayView<const uint8_t>, 2>;
			using Storage = std::vector< obu_payload>;// all_data;
			uint8 * data_ = NULL;
			size_t size_ = 0;
			size_t max_size_ = 0;

			Storage all_data;

			//Storage data_;
		protected:
		private:
		};

		class carray_of_array_views::const_iterator
		{
		public:
		public:
			const_iterator() = default;
			const_iterator(const const_iterator&) = default;
			const_iterator& operator=(const const_iterator&) = default;

			const_iterator& operator++() {
				if (++inner_ == outer_->size/*outer_->size()*/) {
					++outer_;
					inner_ = 0;
				}
				return *this;
			}
			uint8_t operator*() const { return (outer_->start_address)[inner_]; }

			friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) {
				return lhs.outer_ == rhs.outer_ && lhs.inner_ == rhs.inner_;
			}

		private:
			friend carray_of_array_views;
			const_iterator(carray_of_array_views::Storage::const_iterator outer, size_t inner)
				: outer_(outer), inner_(inner) {}

			Storage::const_iterator outer_;
			size_t inner_;
		protected:
		private:
		};




		carray_of_array_views::const_iterator carray_of_array_views::begin() const {
			return const_iterator(all_data.begin(), 0);
		}

		carray_of_array_views::const_iterator carray_of_array_views::end() const {
			return const_iterator(all_data.end(), 0);
		}

		void carray_of_array_views::CopyTo(uint8_t* destination, const_iterator first) const 
		{
			if (first == end())
			{
				// Empty OBU payload. E.g. Temporal Delimiters are always empty.
				return;
			}
			size_t first_chunk_size = first.outer_->size - first.inner_;
			memcpy(destination, first.outer_->start_address + first.inner_, first_chunk_size);
			destination += first_chunk_size;
			for (auto it = std::next(first.outer_); it != all_data.end(); ++it) 
			{
				memcpy(destination, it->start_address, it->size);
				destination += it->size;
			}
		}
 
	struct ObuInfo {
		// Size of the obu_header and obu_size fields in the ouput frame.
		size_t prefix_size = 0;
		// obu_header() and obu_size (leb128 encoded payload_size).
		// obu_header can be up to 2 bytes, obu_size - up to 5.
		std::array<uint8_t, 7> prefix;
		// Size of the obu payload in the output frame, i.e. excluding header
		size_t payload_size = 0;
		// iterator pointing to the beginning of the obu payload.
		carray_of_array_views::const_iterator payload_offset;
		// OBU payloads as written in the rtp packet payloads.
		carray_of_array_views data;
		//cvcm_packet  data;


		//uint8*			payload_offset;

		/*uint8*			data = NULL;
		size_t			data_size = 0;
		size_t			max_data_size = 0;*/
	};

	
	// Reorgonizes array of rtp payloads into array of obus:
// fills ObuInfo::data field.
// Returns empty vector on error.
	std::vector<ObuInfo> ParseObus(const std::vector<cvcm_packet>& rtp_payloads)
	{
		std::vector<ObuInfo> obu_infos;

		size_t  all_obs_data_size = 0;;
		for (const cvcm_packet & packet : rtp_payloads)
		{
			all_obs_data_size += packet.m_size_bytes;
		}
		bool expect_continues_obu = false;
		for (const cvcm_packet & packet : rtp_payloads)
		{
			cbyte_buffer_reader payload(reinterpret_cast<const char*>(packet.m_data_ptr), packet.m_size_bytes);
			uint8_t aggregation_header;
			if (!payload.ReadUInt8(&aggregation_header)) {
				//RTC_DLOG(LS_WARNING)
				//	<< "Failed to find aggregation header in the packet.";
				WARNING_EX_LOG("Failed to find aggregation header in the packet.");
				return obu_infos;
			}
			// Z-bit: 1 if the first OBU contained in the packet is a continuation of a
   // previous OBU.
			bool continues_obu = RtpStartsWithFragment(aggregation_header);
			if (continues_obu != expect_continues_obu) {
				//	RTC_DLOG(LS_WARNING) << "Unexpected Z-bit " << continues_obu;
				WARNING_EX_LOG("Unexpected Z-bit ", continues_obu);
				return obu_infos;
			}
			int num_expected_obus = RtpNumObus(aggregation_header);
			if (payload.Length() == 0) {
				// rtp packet has just the aggregation header. That may be valid only when
				// there is exactly one fragment in the packet of size 0.
				if (num_expected_obus != 1) {
					//RTC_DLOG(LS_WARNING)
						//<< "Invalid packet with just an aggregation header.";
					WARNING_EX_LOG("Invalid packet with just an aggregation header.");
					return obu_infos;
				}
				if (!continues_obu) {
					// Empty packet just to notify there is a new OBU.
					obu_infos.emplace_back();
				}
				expect_continues_obu = RtpEndsWithFragment(aggregation_header);
				continue;
			}

			for (int obu_index = 1; payload.Length() > 0; ++obu_index) 
			{
				/*ObuInfo& obu_info = (obu_index == 1 && continues_obu)
					? obu_infos.back() : obu_infos.emplace_back() ;*/
				uint64_t fragment_size;
				// When num_expected_obus > 0, last OBU (fragment) is not preceeded by
				// the size field. See W field in
				// https://aomediacodec.github.io/av1-rtp-spec/#43-av1-aggregation-header
				bool has_fragment_size = (obu_index != num_expected_obus);
				if (has_fragment_size) {
					if (!payload.ReadUVarint(&fragment_size)) {
						//RTC_DLOG(LS_WARNING) << "Failed to read fragment size for obu #"
							//<< obu_index << "/" << num_expected_obus;
						WARNING_EX_LOG("Failed to read fragment size for obu # %u / %u", obu_index, num_expected_obus);
						return obu_infos;
					}
					if (fragment_size > payload.Length()) {
						// Malformed input: written size is larger than remaining buffer.
						/*RTC_DLOG(LS_WARNING) << "Malformed fragment size " << fragment_size
							<< " is larger than remaining size "
							<< payload.Length() << " while reading obu #"
							<< obu_index << "/" << num_expected_obus;*/
						WARNING_EX_LOG("Malformed fragment size %u is larger than remaining size %u  while reading obu # %u / %u", fragment_size, payload.Length(), obu_index, num_expected_obus);
						return obu_infos;
					}
				}
				else {
					fragment_size = payload.Length();
				}
				// While it is in-practical to pass empty fragments, it is still possible.
				if (fragment_size > 0) {
					//obu_info.data.Append(reinterpret_cast<const uint8_t*>(payload.Data()),
					//	fragment_size);
					/*ObuInfo& obu_info = (obu_index == 1 && continues_obu)
						? obu_infos.back() : obu_infos.emplace_back(); */
					if (obu_index != 1 || !continues_obu)
					{
						obu_infos.emplace_back();
					}
					ObuInfo& obu_info = obu_infos.back();
					if (!obu_info.data.is_init()  )
					{
						 
						if (!obu_info.data.alloc(all_obs_data_size))
						{
							ERROR_EX_LOG("alloc failed !!! [obs size = %u]", all_obs_data_size);
							return obu_infos;
						}
						 
					}
					obu_info.data.Append(reinterpret_cast<const uint8_t*>(payload.Data()), fragment_size);
					//memcpy(obu_info.data + obu_info.data_size, reinterpret_cast<const uint8_t*>(payload.Data()), fragment_size);
					//obu_info.data_size += fragment_size;
					//obu_info.data = packet;
					payload.Consume(fragment_size);
				}
			}
			// Z flag should be same as Y flag of the next packet.
			expect_continues_obu = RtpEndsWithFragment(aggregation_header);
		}
		if (expect_continues_obu)
		{
			//RTC_DLOG(LS_WARNING) << "Last packet shouldn't have last obu fragmented.";
			WARNING_EX_LOG("Last packet shouldn't have last obu fragmented.");
			return obu_infos;
		}
		return obu_infos;

	}

	// Returns number of bytes consumed.
	int WriteLeb128(uint32_t value, uint8_t* buffer) 
	{
		int size = 0;
		while (value >= 0x80) 
		{
			buffer[size] = 0x80 | (value & 0x7F);
			++size;
			value >>= 7;
		}
		buffer[size] = value;
		++size;
		return size;
	}

	// Calculates sizes for the Obu, i.e. base on ObuInfo::data field calculates
	// all other fields in the ObuInfo structure.
	// Returns false if obu found to be misformed.
	bool CalculateObuSizes(ObuInfo* obu_info)
	{
		if (obu_info->data.empty()) 
		{
			//RTC_DLOG(LS_WARNING) << "Invalid bitstream: empty obu provided.";
			WARNING_EX_LOG("Invalid bitstream: empty obu provided.");
			return false;
		}
		carray_of_array_views::const_iterator  it = obu_info->data.begin();
		//uint8* end = obu_info->data + obu_info->data_size;
		uint8_t obu_header = *it;
		obu_info->prefix[0] = obu_header | kObuSizePresentBit;
		obu_info->prefix_size = 1;
		++it;
		if (ObuHasExtension(obu_header)) {
			if (it == obu_info->data.end())
			{
				return false;
			}
			obu_info->prefix[1] = *it;  // obu_extension_header
			obu_info->prefix_size = 2;
			++it;
		}
		// Read, validate, and skip size, if present.
		if (!ObuHasSize(obu_header)) 
		{
			obu_info->payload_size = obu_info->data.size() - obu_info->prefix_size;
		}
		else {
			// Read leb128 encoded field obu_size.
			uint64_t obu_size_bytes = 0;
			// Number of bytes obu_size field occupy in the bitstream.
			int size_of_obu_size_bytes = 0;
			uint8_t leb128_byte;
			do {
				if (it == obu_info->data.end() || size_of_obu_size_bytes >= 8)
				{
					//RTC_DLOG(LS_WARNING)
						//<< "Failed to read obu_size. obu_size field is too long: "
						//<< size_of_obu_size_bytes << " bytes processed.";
					WARNING_EX_LOG("Failed to read obu_size. obu_size field is too long: [%u]  bytes processed.", size_of_obu_size_bytes);
					return false;
				}
				leb128_byte = *it;
				obu_size_bytes |= uint64_t{ leb128_byte & 0x7Fu }
				<< (size_of_obu_size_bytes * 7);
				++size_of_obu_size_bytes;
				++it;
			} while ((leb128_byte & 0x80) != 0);

			obu_info->payload_size = obu_info->data.size() - obu_info->prefix_size - size_of_obu_size_bytes;
			if (obu_size_bytes != obu_info->payload_size) {
				// obu_size was present in the bitstream and mismatches calculated size.
				//RTC_DLOG(LS_WARNING) << "Mismatch in obu_size. signaled: "
					//<< obu_size_bytes
					//<< ", actual: " << obu_info->payload_size;
				WARNING_EX_LOG("Mismatch in obu_size. signaled: [%u], actual: [%u]", obu_size_bytes, obu_info->payload_size);
				return false;
			}
		}
		obu_info->payload_offset = it;
		obu_info->prefix_size +=
			WriteLeb128(static_cast<uint32_t>(obu_info->payload_size), obu_info->prefix.data() + obu_info->prefix_size);
		return true;
	}

	void return_obs_info(std::vector<ObuInfo>& obu_infos)
	{
		for (ObuInfo& obu_info: obu_infos)
		{
			if (obu_info.data.is_init())
			{
				obu_info.data.free();
			}
		}
	}

	std::vector<cvcm_encoded_frame> cvideo_rtp_depacketizer_av1::AssembleFrame(const std::vector<cvcm_packet>& rtp_datas)
	{
		std::vector<ObuInfo>  obu_infos = ParseObus(rtp_datas);
		std::vector<cvcm_encoded_frame>   encoded_buffer;
		if (obu_infos.empty()) {
			return encoded_buffer;
		}
		//完整一帧数据的大小
		size_t frame_size = 0;
		for (ObuInfo& obu_info : obu_infos)
		{
			if (!CalculateObuSizes(&obu_info)) 
			{
				return encoded_buffer;
			}
			frame_size += (obu_info.prefix_size + obu_info.payload_size);
		}


		cvcm_encoded_frame  encoded_frame;
		if (!encoded_frame.alloc(frame_size))
		{
			WARNING_EX_LOG("alloc one encoded frame size [%u] failed !!!", frame_size);
			return_obs_info(obu_infos);
			return encoded_buffer;
		}
		uint8* write_at = encoded_frame.data();
		for (const ObuInfo& obu_info : obu_infos)
		{
			// Copy the obu_header and obu_size fields.
			memcpy(write_at, obu_info.prefix.data(), obu_info.prefix_size);
			write_at += obu_info.prefix_size;
			// Copy the obu payload.
			obu_info.data.CopyTo(write_at, obu_info.payload_offset);
			write_at += obu_info.payload_size;
		}
		return_obs_info(obu_infos);
		encoded_buffer.emplace_back(encoded_frame);
		return encoded_buffer;
	}
}