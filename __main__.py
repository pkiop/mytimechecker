from flask import Flask, render_template
from flask_bootstrap import Bootstrap
import json
import pymysql 
import codecs

app = Flask(__name__)
Bootstrap(app)

# Make the WSGI interface available at the top level so wfastcgi can get it.
wsgi_app = app.wsgi_app
front = """
<!DOCTYPE html>
<!-- saved from url=(0049)http://bootstrapk.com/examples/starter-template/# -->
<html lang="en">

<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">

	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->
	<meta name="description" content="">
	<meta name="author" content="">
	<link rel="icon" href="http://bootstrapk.com/favicon.ico">

	<title>BiteLab</title>

	<!-- Bootstrap core CSS -->
	<link href="/static/css/bootstrap.min.css" rel="stylesheet">

	<!-- Custom styles for this template -->
	<link href="/static/css/starter-template.css" rel="stylesheet">

<body>

	<nav class="navbar navbar-inverse navbar-fixed-top">
		<div class="container">
			<div class="navbar-header">
				<button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#navbar"
					aria-expanded="false" aria-controls="navbar">
					<span class="sr-only">Toggle navigation</span>
					<span class="icon-bar"></span>
					<span class="icon-bar"></span>
					<span class="icon-bar"></span>
				</button>
				<a class="navbar-brand"
					href="https://naver.com">time checker</a>
			</div>
			<div id="navbar" class="collapse navbar-collapse">
				<ul class="nav navbar-nav">
					<li class="active"><a href="http://164.125.63.208:5555/">main</a></li>
					<li><a href="http://164.125.63.208:5555/DBget">time record</a></li>
					<li><a href="http://164.125.63.208:5555/bootstrap">empty</a></li>
				</ul>
			</div>
			<!--/.nav-collapse -->
		</div>
	</nav>

"""

back = """
	<!-- Bootstrap core JavaScript
																																																																								      ================================================== -->
	<!-- Placed at the end of the document so the pages load faster -->
	<script src="/static/js/jquery.min.js"></script>
	<script src="/static/js/bootstrap.min.js"></script>

</body>

</html>
"""
@app.route('/DBget')
def DBget():
    conn = pymysql.connect(host='192.168.1.183', user='root', password='aaaa', db='pkiop_planner', charset='utf8')
    curs = conn.cursor()
    curs.execute("select * from result")
    rows = curs.fetchall()
    result = front
    result += """
        <table>
         <thead>
            <tr>
            <th>ID</th>
            <th>YEAR</th>
            <th>MONTH</th>
            <th>DAY</th>
            <th>PERCENT</th>
            <th>SLEEP</th>
            <th>EXERCISE</th>
            <th>WEEKNUM</th>
            <th>YOIL</th>
            </tr>
         </thead>
         <tbody>
    """
    for x in rows:
        result += "<tr>"
        for y in x:
            result+="<td>"
            result+=str(y)
            result+="</td>"
        result += "</tr>"

    result+="</tbody>"
    result+="</table>"

	result += back
    curs.close()
    conn.close()
    return result
    
@app.route('/pTEXT')
def pTEXT():
    curs = conn.cursor()
    curs.execute("select * from pTEXT")
    rows = curs.fetchall()
    result = """
    <!doctype html>
        <html lang="ko">
<head>
	<meta charset="utf-8">
	<title>OO</title>
</head>
    """
    result
    for x in rows:
        for y in x:
	    print type(result)
            result+=str(y).encode('utf-8')
    curs.close()
    return result

@app.route('/')
def main():
    return render_template('index.html') 

@app.route('/bootstrap')
def bootstrap():
    return render_template('bootstrap.html') 


@app.route('/DB')
def db_access():
    return "It's db"

if __name__ == '__main__':
    import os
    HOST = os.environ.get('SERVER_HOST', '0.0.0.0')
    try:
        PORT = int(os.environ.get('SERVER_PORT', '5555'))
    except ValueError:
        PORT = 5555
    app.run(HOST, PORT)
