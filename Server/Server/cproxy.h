/********************************************************************
created:	2021-11-24

author:		chensong

purpose:	Location

*********************************************************************/

#ifndef API_PROXY_H_
#define API_PROXY_H_

#include <memory>
#include <string>
#include <utility>


#include "event.h"
#include "cmessage_handler.h"
//#include "cthread.h"
#include "clocation.h"

//#include "cthread_pool_mgr.h"
//#include "cdecode_interface.h"
//#include "Export.h"
#include "chttp_queue_mgr.h"
namespace chen
{

	

	template <class T>
	class scoped_refptr {
	public:
		typedef T element_type;

		scoped_refptr() : ptr_(nullptr) {}

		scoped_refptr(T* p) : ptr_(p) {  // NOLINT(runtime/explicit)
			if (ptr_)
				ptr_->AddRef();
		}

		scoped_refptr(const scoped_refptr<T>& r) : ptr_(r.ptr_) {
			if (ptr_)
				ptr_->AddRef();
		}

		template <typename U>
		scoped_refptr(const scoped_refptr<U>& r) : ptr_(r.get()) {
			if (ptr_)
				ptr_->AddRef();
		}

		// Move constructors.
		scoped_refptr(scoped_refptr<T>&& r) : ptr_(r.release()) {}

		template <typename U>
		scoped_refptr(scoped_refptr<U>&& r) : ptr_(r.release()) {}

		~scoped_refptr() {
			if (ptr_)
				ptr_->Release();
		}

		T* get() const { return ptr_; }
		operator T*() const { return ptr_; }
		T* operator->() const { return ptr_; }

		// Returns the (possibly null) raw pointer, and makes the scoped_refptr hold a
		// null pointer, all without touching the reference count of the underlying
		// pointed-to object. The object is still reference counted, and the caller of
		// release() is now the proud owner of one reference, so it is responsible for
		// calling Release() once on the object when no longer using it.
		T* release() {
			T* retVal = ptr_;
			ptr_ = nullptr;
			return retVal;
		}

		scoped_refptr<T>& operator=(T* p) {
			// AddRef first so that self assignment should work
			if (p)
				p->AddRef();
			if (ptr_)
				ptr_->Release();
			ptr_ = p;
			return *this;
		}

		scoped_refptr<T>& operator=(const scoped_refptr<T>& r) {
			return *this = r.ptr_;
		}

		template <typename U>
		scoped_refptr<T>& operator=(const scoped_refptr<U>& r) {
			return *this = r.get();
		}

		scoped_refptr<T>& operator=(scoped_refptr<T>&& r) {
			scoped_refptr<T>(std::move(r)).swap(*this);
			return *this;
		}

		template <typename U>
		scoped_refptr<T>& operator=(scoped_refptr<U>&& r) {
			scoped_refptr<T>(std::move(r)).swap(*this);
			return *this;
		}

		void swap(T** pp) {
			T* p = ptr_;
			ptr_ = *pp;
			*pp = p;
		}

		void swap(scoped_refptr<T>& r) { swap(&r.ptr_); }

	protected:
		T* ptr_;
	};
	///

	template <typename R>
	class ReturnType {
	public:
		template <typename C, typename M>
		void Invoke(C* c, M m) {
			r_ = (c->*m)();
		}
		template <typename C, typename M, typename T1>
		void Invoke(C* c, M m, T1 a1) {
			r_ = (c->*m)(std::move(a1));
		}
		template <typename C, typename M, typename T1, typename T2>
		void Invoke(C* c, M m, T1 a1, T2 a2) {
			r_ = (c->*m)(std::move(a1), std::move(a2));
		}
		template <typename C, typename M, typename T1, typename T2, typename T3>
		void Invoke(C* c, M m, T1 a1, T2 a2, T3 a3) {
			r_ = (c->*m)(std::move(a1), std::move(a2), std::move(a3));
		}
		template <typename C,
			typename M,
			typename T1,
			typename T2,
			typename T3,
			typename T4>
			void Invoke(C* c, M m, T1 a1, T2 a2, T3 a3, T4 a4) {
			r_ = (c->*m)(std::move(a1), std::move(a2), std::move(a3), std::move(a4));
		}
		template <typename C,
			typename M,
			typename T1,
			typename T2,
			typename T3,
			typename T4,
			typename T5>
			void Invoke(C* c, M m, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5) {
			r_ = (c->*m)(std::move(a1), std::move(a2), std::move(a3), std::move(a4),
				std::move(a5));
		}

		R moved_result() { return std::move(r_); }

	private:
		R r_;
	};

	template <>
	class ReturnType<void> {
	public:
		template <typename C, typename M>
		void Invoke(C* c, M m) {
			(c->*m)();
		}
		template <typename C, typename M, typename T1>
		void Invoke(C* c, M m, T1 a1) {
			(c->*m)(std::move(a1));
		}
		template <typename C, typename M, typename T1, typename T2>
		void Invoke(C* c, M m, T1 a1, T2 a2) {
			(c->*m)(std::move(a1), std::move(a2));
		}
		template <typename C, typename M, typename T1, typename T2, typename T3>
		void Invoke(C* c, M m, T1 a1, T2 a2, T3 a3) {
			(c->*m)(std::move(a1), std::move(a2), std::move(a3));
		}

		void moved_result() {}
	};


	namespace internal {

		class SynchronousMethodCall : public  MessageHandler {
		public:
			explicit SynchronousMethodCall( MessageHandler* proxy);
			~SynchronousMethodCall() override;

			void Invoke(const Location& posted_from, chttp_queue_mgr* t);
			void OnMessage(Message*) override;
		private:
			

			Event e_;
			MessageHandler* proxy_;
		};

	}  // namespace internal




	template <typename C, typename R>
	class MethodCall0 : public MessageHandler {
	public:
		typedef R (C::*Method)();
		MethodCall0(C* c, Method m) : c_(c), m_(m) {}

		R Marshal(const chen::Location& posted_from, chttp_queue_mgr* t) {
			internal::SynchronousMethodCall(this).Invoke(posted_from , t);
			return r_.moved_result();
		}

	private: 
		void OnMessage( Message*) { r_.Invoke(c_, m_); }

		C* c_;
		Method m_;
		ReturnType<R> r_;
	};

	template <typename C, typename R>
	class ConstMethodCall0 : public  MessageHandler {
	public:
		typedef R (C::*Method)() const;
		ConstMethodCall0(C* c, Method m) : c_(c), m_(m) {}

		R Marshal(const chen::Location& posted_from, chttp_queue_mgr* t) {
			internal::SynchronousMethodCall(this).Invoke(posted_from , t);
			return r_.moved_result();
		}

	private:
		void OnMessage(Message*) { r_.Invoke(c_, m_); }

		C* c_;
		Method m_;
		ReturnType<R> r_;
	};

	template <typename C, typename R, typename T1>
	class MethodCall1 : public  MessageHandler {
	public:
		typedef R (C::*Method)(T1 a1);
		MethodCall1(C* c, Method m, T1 a1) : c_(c), m_(m), a1_(std::move(a1)) {}

		R Marshal(const  Location& posted_from, chttp_queue_mgr* t ) {
			internal::SynchronousMethodCall(this).Invoke( posted_from  , t );
			return r_.moved_result();
		}

	private:
		void OnMessage(Message* ) { r_.Invoke(c_, m_, std::move(a1_)); }

		C* c_;
		Method m_;
		ReturnType<R> r_;
		T1 a1_;
	};

	template <typename C, typename R, typename T1>
	class ConstMethodCall1 : public  MessageHandler {
	public:
		typedef R (C::*Method)(T1 a1) const;
		ConstMethodCall1(C* c, Method m, T1 a1) : c_(c), m_(m), a1_(std::move(a1)) {}

		R Marshal(const chen::Location& posted_from, chttp_queue_mgr* t ) {
			internal::SynchronousMethodCall(this).Invoke(posted_from  , t  );
			return r_.moved_result();
		}

	private:
		void OnMessage(Message* ) { r_.Invoke(c_, m_, std::move(a1_)); }

		C* c_;
		Method m_;
		ReturnType<R> r_;
		T1 a1_;
	};

	template <typename C, typename R, typename T1, typename T2>
	class MethodCall2 : public  MessageHandler {
	public:
		typedef R (C::*Method)(T1 a1, T2 a2);
		MethodCall2(C* c, Method m, T1 a1, T2 a2)
			: c_(c), m_(m), a1_(std::move(a1)), a2_(std::move(a2)) {}

		R Marshal(const chen::Location& posted_from, chttp_queue_mgr* t ) {
			internal::SynchronousMethodCall(this).Invoke(posted_from  , t  );
			return r_.moved_result();
		}
		/*void OnMessage() {
			r_.Invoke(c_, m_, std::move(a1_), std::move(a2_));
		}*/
	private:
		void OnMessage(Message *) {
			r_.Invoke(c_, m_, std::move(a1_), std::move(a2_));
		}

		C* c_;
		Method m_;
		ReturnType<R> r_;
		T1 a1_;
		T2 a2_;
	};

	template <typename C, typename R, typename T1, typename T2, typename T3>
	class MethodCall3 : public  MessageHandler {
	public:
		typedef R (C::*Method)(T1 a1, T2 a2, T3 a3);
		MethodCall3(C* c, Method m, T1 a1, T2 a2, T3 a3)
			: c_(c),
			m_(m),
			a1_(std::move(a1)),
			a2_(std::move(a2)),
			a3_(std::move(a3)) {}

		R Marshal(const chen::Location& posted_from, chttp_queue_mgr* t ) {
			internal::SynchronousMethodCall(this).Invoke(posted_from  , t  );
			return r_.moved_result();
		}

	private:
		void OnMessage(Message* ) {
			r_.Invoke(c_, m_, std::move(a1_), std::move(a2_), std::move(a3_));
		}

		C* c_;
		Method m_;
		ReturnType<R> r_;
		T1 a1_;
		T2 a2_;
		T3 a3_;
	};

	template <typename C,
		typename R,
		typename T1,
		typename T2,
		typename T3,
		typename T4>
		class MethodCall4 : public MessageHandler {
		public:
			typedef R (C::*Method)(T1 a1, T2 a2, T3 a3, T4 a4);
			MethodCall4(C* c, Method m, T1 a1, T2 a2, T3 a3, T4 a4)
				: c_(c),
				m_(m),
				a1_(std::move(a1)),
				a2_(std::move(a2)),
				a3_(std::move(a3)),
				a4_(std::move(a4)) {}

			R Marshal(const chen::Location& posted_from, chttp_queue_mgr* t) {
				internal::SynchronousMethodCall(this).Invoke(posted_from  , t );
				return r_.moved_result();
			}

		private:
			void OnMessage(Message*) {
				r_.Invoke(c_, m_, std::move(a1_), std::move(a2_), std::move(a3_),
					std::move(a4_));
			}

			C* c_;
			Method m_;
			ReturnType<R> r_;
			T1 a1_;
			T2 a2_;
			T3 a3_;
			T4 a4_;
	};

	template <typename C,
		typename R,
		typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
		class MethodCall5 : public MessageHandler {
		public:
			typedef R (C::*Method)(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5);
			MethodCall5(C* c, Method m, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
				: c_(c),
				m_(m),
				a1_(std::move(a1)),
				a2_(std::move(a2)),
				a3_(std::move(a3)),
				a4_(std::move(a4)),
				a5_(std::move(a5)) {}

			R Marshal(const chen::Location& posted_from, chttp_queue_mgr* t) {
				internal::SynchronousMethodCall(this).Invoke(posted_from  , t );
				return r_.moved_result();
			}

		private:
			void OnMessage(Message*) {
				r_.Invoke(c_, m_, std::move(a1_), std::move(a2_), std::move(a3_),
					std::move(a4_), std::move(a5_));
			}

			C* c_;
			Method m_;
			ReturnType<R> r_;
			T1 a1_;
			T2 a2_;
			T3 a3_;
			T4 a4_;
			T5 a5_;
	};


#define GET_OBJECT_THREAD(o)
                                                 
//#define GET_OBJECT_THREAD(o)													\
//	Object *object = g_cthread_pool_mgr.get_object_thread(o);					\
//	if (!object)																\
//	{																			\
//		return EDecode_Error_Code_Mem;											\
//	}

	

#define BEGIN_PROXY_MAP(c)														\
    class c##_proxy {															\
	typedef c##_interface		C;												\
	public:																		\
		c##_proxy(){}															\
		~c##_proxy(){}															\
	public:



#define PROXY_WORKER_METHOD0(r, method)												\
  r method()  {																	\
     MethodCall0<C, r> call(&g_web_http_api_mgr , &C::method);								\
	r r1 = call.Marshal(CHEN_FROM_HERE, &g_http_queue_mgr);												\
	return 	r1;																							\
   }
   
   
#define PROXY_WORKER_METHOD1(r, method, t1 )											\
  r method(t1 a1 )  {																\
    MethodCall1<C, r, t1> call(&g_web_http_api_mgr , &C::method, std::move(a1));        \
	r r1 = call.Marshal(CHEN_FROM_HERE, &g_http_queue_mgr);												\
	return 	r1;																							\
  }

#define PROXY_WORKER_METHOD2(r, method, t1, t2 )										\
  r method(t1 a1, t2 a2 )  {														\
    MethodCall2<C, r, t1, t2> call(&g_web_http_api_mgr , &C::method,  std::move(a1), std::move(a2));					\
	r r1 = call.Marshal(CHEN_FROM_HERE, &g_http_queue_mgr);												\
	return 	r1;																							\
  }

#define PROXY_WORKER_METHOD5(r, method, t1, t2, t3, t4, t5)												\
  r method(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5)  {														\
  	CHEN_PROXY_WORK_START_TIME																			\
		GET_OBJECT_THREAD(a1)																			\
    MethodCall4<C, r, t2, t3, t4, t5> call(object->m_object , &C::method,								\
                                       std::move(a2), std::move(a3), std::move(a4), std::move(a5));		\
	r r1 = call.Marshal(CHEN_FROM_HERE, &g_http_queue_mgr);												\
	CHEN_PROXY_WORK_END_TIME_MS																			\
	return 	r1;																							\
  }



#define END_PROXY_MAP()																	\
  };
} //namespace chen

#endif  //  API_PROXY_H_
