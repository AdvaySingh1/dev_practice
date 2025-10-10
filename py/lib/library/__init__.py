import sys
from collections import defaultdict

class book:
    def __init__(self, name, author):
        self.name = name
        self.author = author
    def __str__(self):
        print(f"Title: {self.name}. Written by {self.author}.")
class library:

    # would need num borrowed logic so you don't return somewhere else

    def __init__(self, **kargs):
        self.books = {} # key = book, count
        self.books = defaultdict(lambda: [0, ""])
    
    def add_book(self, name: str, count: int, author: str):
        self.books[name] = [self.books[name][0] + count, author]
        

    def borrow_book(self, name: str) -> bool:
        if (name in self.books and self.books[name][0]):
            self.books[name][0] -= 1
            return True
        return False

    def return_book(self, name: str) -> bool:
        if (name in self.books and self.books[name][0]):
            self.books[name][0] += 1
            return True
        return False
    
    def view_books(self):
        for book, pair in self.books.items():
            count, author = pair
            print(f"Book: {book}. Written by {author}. In stock: {count}.")
    



