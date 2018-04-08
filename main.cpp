#include <iostream>

using namespace std;

    template <class T>
        void default_delete(T *p)
        {
                delete p;
        }

        template <class T>
        void default_array_delete(T *p)
        {
                delete [] p;
        }

        template <class T>
        class shared_ptr
        {
                using pointer = T*;
                using reference = T&;
                using delete_fn = void (*) (pointer);
                using counter = int;

                pointer ptr;
                counter *cnt;
                delete_fn del;

        public:
                shared_ptr()
                        : ptr(nullptr), cnt(nullptr), del( default_delete<T> )
                {}

                 shared_ptr(pointer p)
                        : ptr(p), cnt( new counter(1) ), del( default_delete<T> )
                {}

                shared_ptr(pointer p, delete_fn d)
                        : ptr(p), cnt( new counter(1) ), del(d)
                {}

                shared_ptr(const shared_ptr &x)
                        : shared_ptr(x.ptr, x.del)
                {
                        cnt = x.cnt;
                        ++*cnt;
                }

                shared_ptr(shared_ptr &&x)
                        : ptr( move(x.ptr) ),
                          cnt( move(x.cnt) ),
                          del( move(x.del) )
                {
                        x.ptr = nullptr;
                        x.cnt = nullptr;
                }

                shared_ptr& operator = (const shared_ptr &x)
                {
                        if (this != &x) {
                                reset();
                                ptr = x.ptr;
                                cnt = x.cnt;
                                del = x.del;
                                ++*cnt;
                        }
                        return *this;
                }

                shared_ptr& operator = (shared_ptr &&x)
                {
                        swap_elem(x);
                        return *this;
                }

                void swap_elem(shared_ptr &x)
                {
                        swap(ptr, x.ptr);
                        swap(cnt, x.cnt);
                        swap(del, x.del);
                }

                ~shared_ptr()
                {
                        if (cnt != nullptr && --*cnt == 0) {
                                cout << " destroyed " << *ptr << "\n";
                                del(ptr);
                                delete cnt;
                        }
                }

                pointer operator -> () const
                {
                        return ptr;
                }

                reference operator * () const
                {
                        return *ptr;
                }

                pointer get() const
                {
                        return ptr;
                }

                 operator bool() const
                {
                        return ptr != nullptr;
                }

                bool unique() const
                {
                        return use_count() == 1;
                }

                counter use_count() const
                {
                    if (cnt != nullptr)
                        return *cnt;
                    else
                        return 0;

                }

                void reset()
                {
                        this->~shared_ptr();
                        ptr = nullptr;
                        cnt = nullptr;
                }

                void reset(pointer p)
                {
                        reset();
                        cnt = new counter(1);
                        ptr = p;
                }
                void reset(pointer p, delete_fn d)
                {
                        reset();
                        cnt = new counter(1);
                        ptr = p;
                        del = d;
                }
        };

        template <class T, class... Args>
        shared_ptr<T> make_shared(Args&& ...args)
        {
                return shared_ptr<T>( new T( forward<Args>(args)... ) );
        }

int main()
{
    shared_ptr<int> foo;
    shared_ptr<int> bar (new int(10));

    foo = bar;

    bar = make_shared<int> (20);

    bar.reset();

    foo.reset(new int(30), default_delete);
    cout << *foo << endl;
    bar = foo;
    cout << bar.use_count() << endl;
    return 0;
}
