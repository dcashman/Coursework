[Attack C]
Part C: The html file in part C uses a SQL injection attack on
zoobar.org to login a user without using a password.  This is possible
because the site, zoobar.org does not properly sanitize its SQL
queries in all places.  Specifically, in _checkLogin() in the auth.php
file, the username is quoted (ostensibly to rid itself of any SQL
injection), but is soon thereafter used in its original form. This is
incredibly stupid on the part of the designers of the site, not
because the SQL is not being quoted, but rather because the SQL is
quoted, for precisely this purpose, and then is not quoted on the SQL
query immediately after the first one in _checkLogin().  _checkLogin
is a part of the flow of the php script any time a person is logging
in or registering.  The program goes from index.php - > common.php ->
login.php -> _checkLogin() (or _addRegistration() -> _checkLogin() if
registering).  The lack of sanitation in the _checkLogin() instruction
allows a user to take the second (unprotected) SQL statement and
modify it however they please by adding characters that would be
interpreted by the SQL engine (in this case sqlite) as commands in
sql.

We exploited this vulernability to allow a user to login to his/her
account without using their password.  The specific query in question,
mentioned above is

$sql = "SELECT * FROM Person WHERE " .  "Username = '$username' AND "
           .  "Password = '$hashedpassword'"; $result =
           $this->db->executeQuery($sql);

This query is used to authenticate the user and log them in.  By
injecting a closing quote and a semi-colon, we can terminate the
statment because the sqlite query will think it is issuing this
command: SELECT * FROM WHERE Username = ''; This will prevent the rest
of the command from being executed.  By adding a '--' to the end, it
will gracefully interpret the rest as comments and we've got our own,
legitimate modified SQL statement.  If we could then get the
Username='' part to become Username='userOfInterest' then we can
execute a query of the form: SELECT * FROM WHERE
Username='userOfIterest'; which will return true and log that user in.
Thus, we set up a form on our html file, c.html, which takes the
user's username and appends a ';-- to the end, thus cutting off the SQL
query and returning a true login no matter which password was entered.
This is done by clicking the "register" button so that the new user,
userOfInterest;'-- is added to the database (using quoted).  When
registering, we assume that this user did not already exist, and thus,
it is added to the database, and then _checkLogin() is entered which
delivers the modified SQL and logs our user in.  This can only be done
once, since the username will exist after the first try and then the
"register" functio will return false.  After the first use, however,
we could then direct it to "login" intsead of "register", but we have
just done it for the first time.  Namely, just the "register" part.


[Attack D]
This html file steals the users password by exploiting an XSS vulnerability that
exists on the zoobar.org site. Specifically, the vulnerability is in the
login.php file, on lines 60-61 where the POST variables are echoed back to the
user. What our attack does is generate a string that, when echoed back, will be
valid javascript which will steal the users password. The implementer of
zoobar.org might think that they are safe from an attack of this sort because
the string that is echoed to the screen is passed through the htmlspecialchars
function first, preventing the creation of new tags in the page through XSS
(since we can't use the < or >). Unfortunately for them, this is not the case,
as we can still do something like the following:

<input .... onload="alert('your website is insecure');">

By utilizing an inline event handler such as onload, we can inject arbitrary
javascript into the page. The only remaining difficulty is that if we want to
use any strings in our code, they will be escaped by htmlspecialchars. However,
we can get around this limitation using something like the String.fromCharCode,
which converts a list of ASCII codes to the equivalent string, avoiding any need
for a quotes (made easy to use with the get_str function in our code). Once we 
can inject such js into the login page, it is a simple matter to create a script
that will listen for the click button of the login button and, when that event
is fired, will utilize the zoomail functionality to steal the users password.
