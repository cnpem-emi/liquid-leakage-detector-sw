from distutils.core import setup, Extension

def main():
    setup(name="LLDet",
          version="1.0.0",
          description="Python interface for leakage detector's pulse function",
          ext_modules=[Extension("LLDet", ["reflexao.c"])])

if __name__ == "__main__":
    main()

