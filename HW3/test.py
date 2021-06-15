from subprocess import STDOUT, check_output as run
from threading import Thread, current_thread


THREAD_COUNT = 20
ITERATION_COUNT = 1


def send_dynamic_requests():
    for i in range(ITERATION_COUNT):
        to_print = ''
        to_print += str(current_thread()) + '\n'
        to_print += 'iteration: ' + str(i) + '\n'
        response = run(args=('/home/student/CLionProjects/OperatingSystemWHW/HW3/client',
                             'localhost', '4096', 'output.cgi'), stderr=STDOUT)
        to_print += response.decode('utf-8')

        print(to_print)


def send_static_requests():
    for i in range(ITERATION_COUNT):
        to_print = ''
        to_print += str(current_thread()) + '\n'
        to_print += 'iteration: ' + str(i) + '\n'
        response = run(args=('/home/student/CLionProjects/OperatingSystemWHW/HW3/client',
                             'localhost', '4096', 'home.html'), stderr=STDOUT)
        to_print += response.decode('utf-8')

        print(to_print)


def init_threads(amount):
    threads = []

    for i in range(amount):
        threads.append(Thread(target=send_dynamic_requests))
        threads.append(Thread(target=send_static_requests))

    return threads


def start_threads(threads):
    for t in threads:
        t.start()


def join_threads(threads):
    for t in threads:
        t.join()


def main():
    print('init threads')
    threads = init_threads(THREAD_COUNT)

    print('starting threads')
    start_threads(threads)

    print('waiting threads')
    join_threads(threads)


if __name__ == '__main__':
    main()
