
//#include "task.h"
#include "uv.h"

#include <stdio.h>
#include <stdlib.h>
//#include <string>
/* Fully close a loop */
static void close_walk_cb(uv_handle_t* handle, void* arg) {
	if (!uv_is_closing(handle))
		uv_close(handle, NULL);
}

 static void close_loop(uv_loop_t* loop) {
	uv_walk(loop, close_walk_cb, NULL);
	uv_run(loop, UV_RUN_DEFAULT);
}
/* This macro cleans up the main loop. This is used to avoid valgrind
 * warnings about memory being "leaked" by the main event loop.
 */
#define MAKE_VALGRIND_HAPPY()                       \
  do {                                              \
    close_loop(uv_default_loop());                  \
    ASSERT(0 == uv_loop_close(uv_default_loop()));  \
    uv_library_shutdown();                          \
  } while (0)

 /* Have our own assert, so we are sure it does not get optimized away in
  * a release build.
  */
#define ASSERT(expr)                                      \
 do {                                                     \
  if (!(expr)) {                                          \
    fprintf(stderr,                                       \
            "Assertion failed in %s on line %d: %s\n",    \
            __FILE__,                                     \
            __LINE__,                                     \
            #expr);                                       \
    abort();                                              \
  }                                                       \
 } while (0)



#define ASSERT_BASE(a, operator, b, type, conv)              \
 do {                                                        \
  volatile type eval_a = (type) (a);                         \
  volatile type eval_b = (type) (b);                         \
  if (!(eval_a operator eval_b)) {                           \
    fprintf(stderr,                                          \
            "Assertion failed in %s on line %d: `%s %s %s` " \
            "(%"conv" %s %"conv")\n",                        \
            __FILE__,                                        \
            __LINE__,                                        \
            #a,                                              \
            #operator,                                       \
            #b,                                              \
            eval_a,                                          \
            #operator,                                       \
            eval_b);                                         \
    abort();                                                 \
  }                                                          \
 } while (0)

#define ASSERT_BASE_STR(expr, a, operator, b, type, conv)      \
 do {                                                          \
  if (!(expr)) {                                               \
    fprintf(stderr,                                            \
            "Assertion failed in %s on line %d: `%s %s %s` "   \
            "(%"conv" %s %"conv")\n",                          \
            __FILE__,                                          \
            __LINE__,                                          \
            #a,                                                \
            #operator,                                         \
            #b,                                                \
            (type)a,                                           \
            #operator,                                         \
            (type)b);                                          \
    abort();                                                   \
  }                                                            \
 } while (0)

#define ASSERT_BASE_LEN(expr, a, operator, b, conv, len)     \
 do {                                                        \
  if (!(expr)) {                                             \
    fprintf(stderr,                                          \
            "Assertion failed in %s on line %d: `%s %s %s` " \
            "(%.*"#conv" %s %.*"#conv")\n",                  \
            __FILE__,                                        \
            __LINE__,                                        \
            #a,                                              \
            #operator,                                       \
            #b,                                              \
            (int)len,                                        \
            a,                                               \
            #operator,                                       \
            (int)len,                                        \
            b);                                              \
    abort();                                                 \
  }                                                          \
 } while (0)

#define ASSERT_BASE_HEX(expr, a, operator, b, size)            \
 do {                                                          \
  if (!(expr)) {                                               \
    int i;                                                     \
    unsigned char* a_ = (unsigned char*)a;                     \
    unsigned char* b_ = (unsigned char*)b;                     \
    fprintf(stderr,                                            \
            "Assertion failed in %s on line %d: `%s %s %s` (", \
            __FILE__,                                          \
            __LINE__,                                          \
            #a,                                                \
            #operator,                                         \
            #b);                                               \
    for (i = 0; i < size; ++i) {                               \
      if (i > 0) fprintf(stderr, ":");                         \
      fprintf(stderr, "%02X", a_[i]);                          \
    }                                                          \
    fprintf(stderr, " %s ", #operator);                        \
    for (i = 0; i < size; ++i) {                               \
      if (i > 0) fprintf(stderr, ":");                         \
      fprintf(stderr, "%02X", b_[i]);                          \
    }                                                          \
    fprintf(stderr, ")\n");                                    \
    abort();                                                   \
  }                                                            \
 } while (0)

#define ASSERT_EQ(a, b) ASSERT_BASE(a, ==, b, int64_t, PRId64)
#define ASSERT_GE(a, b) ASSERT_BASE(a, >=, b, int64_t, PRId64)
#define ASSERT_GT(a, b) ASSERT_BASE(a, >, b, int64_t, PRId64)
#define ASSERT_LE(a, b) ASSERT_BASE(a, <=, b, int64_t, PRId64)
#define ASSERT_LT(a, b) ASSERT_BASE(a, <, b, int64_t, PRId64)
#define ASSERT_NE(a, b) ASSERT_BASE(a, !=, b, int64_t, PRId64)

#define ASSERT_UINT64_EQ(a, b) ASSERT_BASE(a, ==, b, uint64_t, PRIu64)
#define ASSERT_UINT64_GE(a, b) ASSERT_BASE(a, >=, b, uint64_t, PRIu64)
#define ASSERT_UINT64_GT(a, b) ASSERT_BASE(a, >, b, uint64_t, PRIu64)
#define ASSERT_UINT64_LE(a, b) ASSERT_BASE(a, <=, b, uint64_t, PRIu64)
#define ASSERT_UINT64_LT(a, b) ASSERT_BASE(a, <, b, uint64_t, PRIu64)
#define ASSERT_UINT64_NE(a, b) ASSERT_BASE(a, !=, b, uint64_t, PRIu64)

#define ASSERT_DOUBLE_EQ(a, b) ASSERT_BASE(a, ==, b, double, "f")
#define ASSERT_DOUBLE_GE(a, b) ASSERT_BASE(a, >=, b, double, "f")
#define ASSERT_DOUBLE_GT(a, b) ASSERT_BASE(a, >, b, double, "f")
#define ASSERT_DOUBLE_LE(a, b) ASSERT_BASE(a, <=, b, double, "f")
#define ASSERT_DOUBLE_LT(a, b) ASSERT_BASE(a, <, b, double, "f")
#define ASSERT_DOUBLE_NE(a, b) ASSERT_BASE(a, !=, b, double, "f")

#define ASSERT_STR_EQ(a, b) \
  ASSERT_BASE_STR(strcmp(a, b) == 0, a, == , b, char*, "s")

#define ASSERT_STR_NE(a, b) \
  ASSERT_BASE_STR(strcmp(a, b) != 0, a, !=, b, char*, "s")

#define ASSERT_MEM_EQ(a, b, size) \
  ASSERT_BASE_LEN(memcmp(a, b, size) == 0, a, ==, b, s, size)

#define ASSERT_MEM_NE(a, b, size) \
  ASSERT_BASE_LEN(memcmp(a, b, size) != 0, a, !=, b, s, size)

#define ASSERT_MEM_HEX_EQ(a, b, size) \
  ASSERT_BASE_HEX(memcmp(a, b, size) == 0, a, ==, b, size)

#define ASSERT_MEM_HEX_NE(a, b, size) \
  ASSERT_BASE_HEX(memcmp(a, b, size) != 0, a, !=, b, size)

#define ASSERT_NULL(a) \
  ASSERT_BASE(a, ==, NULL, void*, "p")

#define ASSERT_NOT_NULL(a) \
  ASSERT_BASE(a, !=, NULL, void*, "p")

#define ASSERT_PTR_EQ(a, b) \
  ASSERT_BASE(a, ==, b, void*, "p")

#define ASSERT_PTR_NE(a, b) \
  ASSERT_BASE(a, !=, b, void*, "p")

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - offsetof(type, member)))

/* Format big numbers nicely.WARNING: leaks memory. */
//std::string ret;
//const char* fmt(double d)
//{
//	ret = std::string(d);
//	return ret.c_str();
//}

#define NUM_PINGS (1000 * 1000)

struct ctx {
	uv_loop_t loop;
	uv_thread_t thread;
	uv_async_t main_async;    /* wake up main thread */
	uv_async_t worker_async;  /* wake up worker */
	unsigned int nthreads;
	unsigned int main_sent;
	unsigned int main_seen;
	unsigned int worker_sent;
	unsigned int worker_seen;
};


static void worker_async_cb(uv_async_t* handle) {
	struct ctx* ctx = container_of(handle, struct ctx, worker_async);

	ASSERT(0 == uv_async_send(&ctx->main_async));
	ctx->worker_sent++;
	ctx->worker_seen++;

	if (ctx->worker_sent >= NUM_PINGS)
		uv_close((uv_handle_t*)&ctx->worker_async, NULL);
}


static void main_async_cb(uv_async_t* handle) {
	struct ctx* ctx = container_of(handle, struct ctx, main_async);

	ASSERT(0 == uv_async_send(&ctx->worker_async));
	ctx->main_sent++;
	ctx->main_seen++;

	if (ctx->main_sent >= NUM_PINGS)
		uv_close((uv_handle_t*)&ctx->main_async, NULL);
}


static void worker(void* arg) {
	struct ctx* ctx = (struct  ctx*)arg;
	ASSERT(0 == uv_async_send(&ctx->main_async));
	ASSERT(0 == uv_run(&ctx->loop, UV_RUN_DEFAULT));
	uv_loop_close(&ctx->loop);
}


static int test_async(int nthreads) {
	struct ctx* threads;
	struct ctx* ctx;
	uint64_t time;
	int i;

	threads = calloc(nthreads, sizeof(threads[0]));
	ASSERT_NOT_NULL(threads);

	for (i = 0; i < nthreads; i++) {
		ctx = threads + i;
		ctx->nthreads = nthreads;
		ASSERT(0 == uv_loop_init(&ctx->loop));
		ASSERT(0 == uv_async_init(&ctx->loop, &ctx->worker_async, worker_async_cb));
		ASSERT(0 == uv_async_init(uv_default_loop(),
			&ctx->main_async,
			main_async_cb));
		ASSERT(0 == uv_thread_create(&ctx->thread, worker, ctx));
	}

	time = uv_hrtime();

	ASSERT(0 == uv_run(uv_default_loop(), UV_RUN_DEFAULT));

	for (i = 0; i < nthreads; i++)
		ASSERT(0 == uv_thread_join(&threads[i].thread));

	time = uv_hrtime() - time;

	for (i = 0; i < nthreads; i++) {
		ctx = threads + i;
		ASSERT(ctx->worker_sent == NUM_PINGS);
		ASSERT(ctx->worker_seen == NUM_PINGS);
		ASSERT(ctx->main_sent == (unsigned int)NUM_PINGS);
		ASSERT(ctx->main_seen == (unsigned int)NUM_PINGS);
	}

	printf("async%d: %.2f sec  \n",
		nthreads,
		time / 1e9 );

	free(threads);

	MAKE_VALGRIND_HAPPY();
	return 0;
}

int dddmain(int argc, char* argv[])
{
	test_async(2);
	system("pause");
	return 0;
}

/* Just sugar for wrapping the main() for a task or helper. */
#define TEST_IMPL(name)                                                       \
  int run_test_##name(void);                                                  \
  int run_test_##name(void)

#define BENCHMARK_IMPL(name)                                                  \
  int run_benchmark_##name(void);                                             \
  int run_benchmark_##name(void)

#define HELPER_IMPL(name)                                                     \
  int run_helper_##name(void);                                                \
  int run_helper_##name(void)
//
//BENCHMARK_IMPL(async1) {
//	return test_async(1);
//}
//
//
//BENCHMARK_IMPL(async2) {
//	return test_async(2);
//}
//
//
//BENCHMARK_IMPL(async4) {
//	return test_async(4);
//}
//
//
//BENCHMARK_IMPL(async8) {
//	return test_async(8);
//}
