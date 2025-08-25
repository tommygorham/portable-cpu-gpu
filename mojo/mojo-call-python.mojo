# Use Python's NumPy from a Mojo script 
from python import Python
def main():
    # Import the Python 'numpy' module into a variable named 'np'
    np = Python.import_module("numpy")
    # Use np to create an array  
    a = np.arange(15).reshape(3, 5)
    print("Hello from Mojo, with a NumPy array:")
    print(a)
