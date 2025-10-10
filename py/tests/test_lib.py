import sys
sys.path.append('..')
from lib.library import library

def main():

    print("Hello there")
    my_lib = library()

    my_lib.add_book("Harry potter", 10, "JK Rowling")
    my_lib.add_book("Percy Jackson", 5, "Rick Riordan")

    my_lib.view_books()

    for i in range(10):
        print(f"{i} {my_lib.borrow_book('Percy Jackson')}")
    
    my_lib.view_books()

    my_lib.add_book("Harry potter", 10, "Not JK Rowling")

    my_lib.view_books()

    my_lib.return_book("Harry potter")

    my_lib.view_books()


if __name__=="__main__":
    main()