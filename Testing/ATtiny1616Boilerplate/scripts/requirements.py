Import("env")

print("Verifying required packages...")

# Install required packages
try:
    import pymcuprog
except ImportError:
    env.Execute("$PYTHONEXE -m pip install pymcuprog")
