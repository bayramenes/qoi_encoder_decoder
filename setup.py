import os
import platform




def setup():
    print('Starting to setup the enviroments for you')
    os_name = platform.system()
    if os_name == 'Windows':
        print('Windows OS detected')
        print('creating virtual enviroment...')
        if not os.system('python -m venv venv') :
            print('virtual enviroment created...')
            print('installing requirements')
            if not os.system('./venv/bin/python -m pip install -r requirements.txt'):
                print('requirements installed')
                print('Setup finished...')
                print('you can activat the enviroment by running ./venv/bin/activate.ps1')
                exit()
            else:
                print('failed to install requirements')
                print('deleting virtual enviroment')
                os.system('rmdir venv')
                exit()
        else:
            print('failed to create virtual enviroment')
            exit()




    elif os_name=='Linux':
        print('Linux OS detected')
        print('creating virtual enviroment...')
        if not os.system('python3 -m venv venv') :
            print('virtual enviroment created...')
            print('installing requirements')
            if not os.system('./venv/bin/python -m pip install -r requirements.txt'):
                print('requirements installed')
                print('Setup finished...')
                print('you can activat the enviroment by running : source ./venv/bin/activate')
                exit()
            else:
                print('failed to install requirements')
                print('deleting virtual enviroment')
                os.system('rm -rf venv')
                exit()
        else:
            print('failed to create virtual enviroment')
            exit()

    elif os_name=='Darwin':
        print('Mac OS detected')
        print('creating virtual enviroment...')
        if not os.system('python3 -m venv venv') :
            print('virtual enviroment created...')
            print('installing requirements')
            if not os.system('./venv/bin/python -m pip install -r requirements.txt'):
                print('requirements installed')
                print('Setup finished...')
                print('you can activat the enviroment by running : source ./venv/bin/activate')

                exit()
            else:
                print('failed to install requirements')
                print('deleting virtual enviroment')
                os.system('rm -rf venv')
                exit()
        else:
            print('failed to create virtual enviroment')
            exit()
    else:
        print('Unknown OS')
        exit()
    print('Setup finished')
    exit()




def main():
    print('Welcome to the setup program!')
    print('this program will setup the enviroment to be able to convert images to .bmp format so that qoi encoder and decoder can understand the image...')
    print('if you have a .bmp image already you can exit this program and just run the encoder or decoder on the file you have without the need for this program...')
    while True:

        will_continue= input('Do you want to continue? (y/n) ')
        if will_continue == 'n':
            print('Exiting...')
            exit()
        elif will_continue == 'y':
            setup()
        else:
            print('Invalid input try again!!')
            continue
            
    
if __name__ == '__main__':
    main()


