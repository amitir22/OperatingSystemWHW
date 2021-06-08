from subprocess import PIPE, check_output as run
from threading import Thread, current_thread
from os import chdir, getcwd
#from os.path import abspath


path = '/home/student/CLionProjects/OperatingSystemWHW/HW3'


def send_requests():
    for i in range(10):
        print(current_thread())
        print(i)
        ilen = len(run(args=('/home/student/CLionProjects/OperatingSystemWHW/HW3/client',
                             'localhost', '4096', 'big.txt')))
        print(ilen)


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
