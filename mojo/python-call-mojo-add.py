import mojo
def main():
    # Import Mojo function like any other Python module
    from fast_math import add
    x = 5
    y = 10
    # Call the Mojo function
    result = add(x, y)
    
    print(f"Hello from Python")
    print(f"Mojo calculated {x} + {y} = {result}")

if __name__ == "__main__":
    main()
