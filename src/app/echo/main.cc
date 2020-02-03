//
// Created by null on 20-2-1.
//

#include <net/tcp_session.hh>
#include <pump.hh>
#include <utils/lru_cache.hh>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "InfiniteRecursion"

void
wait_ping(net::tcp_session& session){
    session.wait_packet()
            .then([&session](net::linear_ringbuffer_st *buf)mutable{
                session.send_data(buf->read_head(),buf->size())
                        .then([&session,buf,len=buf->size()](){
                            buf->consume(len);
                            wait_ping(session);
                        })
                        .submit();
            })
            .when_exception([&session](std::exception_ptr p){
                try{
                    if(p)
                        std::rethrow_exception(p);
                }
                catch (const net::tcp_session_exception& e){
                    logger::info("tcp session exception :{0} session closed",e.code);
                }
                catch (const std::exception& e){
                    std::cout<<e.what()<<std::endl;
                }
                catch (...){
                    logger::info("cant catch exception in flow::when_exception");
                }
                return false;
            })
            .submit();
}

template<uint16_t _PORT_>
void run_echo(net::listener<_PORT_>& l){
    net::wait_connect(l)
            .then([&l](net::session_data &&session_d) {
                net::tcp_session *s = new net::tcp_session(session_d);
                wait_ping(s->start_at(hw::cpu_core::_01));
                run_echo(l);
            })
            .submit();
}
#pragma clang diagnostic pop

int main(){
    hw::pin_this_thread(0);
    sleep(1);
    hw::the_cpu_count=1;
    engine::init_engine
            <
                    reactor::sortable_task<utils::noncopyable_function<void()>,1>,
                    reactor::sortable_task<utils::noncopyable_function<void()>,2>
            >();
    sleep(1);
    logger::info("echo server started ......");
    run_echo(net::listener<9022>::instance.start_at(hw::cpu_core::_01));
    logger::info("echo server listen at {}",9022);
    sleep(10000);
    return 0;
}