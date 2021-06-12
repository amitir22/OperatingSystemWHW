from subprocess import check_output as run
from threading import Thread, current_thread


def send_requests():
    for i in range(10):
        print(current_thread())
        print('iteration: ' + str(i))
        response = run(args=('/home/student/CLionProjects/OperatingSystemWHW/HW3/client',
                             'localhost', '4096', 'output.cgi'))
        print(response.decode('utf-8'))


def main():
    print('init threads')
    t1 = Thread(target=send_requests)
    t2 = Thread(target=send_requests)
    t3 = Thread(target=send_requests)
    t4 = Thread(target=send_requests)

    print('starting threads')
    t1.start()
    t2.start()
    t3.start()
    t4.start()

    t1.join()
    t2.join()
    t3.join()
    t4.join()


if __name__ == '__main__':
    main()
