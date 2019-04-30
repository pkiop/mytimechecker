# -*- coding: utf-8 -*-

#server, db
from flask import Flask, render_template
from flask_bootstrap import Bootstrap
import json
import pymysql 
import codecs

#crawler
import sys
import io
from bs4 import BeautifulSoup
import requests
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
import time



app = Flask(__name__)
Bootstrap(app)


chrome_options = Options()
chrome_options.add_argument("--headless")
driver = webdriver.Chrome(chrome_options=chrome_options, executable_path="/usr/lib/chromium-browser/chromedriver")#"home/pi/Project/mytimechecker/crawler/chromedriver")
#driver = webdriver.Chrome("/home/pi/Project/mytimechecker/crawler/chromedriver")
driver.set_page_load_timeout(10)

def init(url):
    driver.get(url)
    driver.implicitly_wait(0.1)
    return driver.page_source

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

	<title>PSS</title>

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
					href="http://164.125.63.208:5555">time checker</a>
			</div>
			<div id="navbar" class="collapse navbar-collapse">
				<ul class="nav navbar-nav">
					<li class="active"><a href="http://164.125.63.208:5555/">main</a></li>
					<li><a href="http://164.125.63.208:5555/DBget">time record</a></li>
					<li><a href="http://164.125.63.208:5555/bootstrap">empty</a></li>
					<li><a href="http://www.pusan.ac.kr/kor/CMS/MenuMgr/menuListOnBuilding.do?">식단</a></li>
					<li><a href="https://bitelab.pusan.ac.kr/ccslab/index.do">Bitelab</a></li>
				</ul>
			</div>
			<!--/.nav-collapse -->
		</div>
	</nav>
    
    <div class="page-header">
        <h1>Carousel</h1>
    </div>
    <div id="carousel-example-generic" class="carousel slide" data-ride="carousel">
    <ol class="carousel-indicators">
        <li data-target="#carousel-example-generic" data-slide-to="0" class="active"></li>
        <li data-target="#carousel-example-generic" data-slide-to="1"></li>
        <li data-target="#carousel-example-generic" data-slide-to="2"></li>
    </ol>
    <div class="carousel-inner" role="listbox">
        <div class="item active">
        <img data-src="holder.js/1140x500/auto/#777:#555/text:First slide" alt="First slide">
        </div>
        <div class="item">
        <img data-src="holder.js/1140x500/auto/#666:#444/text:Second slide" alt="Second slide">
        </div>
        <div class="item">
        <img data-src="holder.js/1140x500/auto/#555:#333/text:Third slide" alt="Third slide">
        </div>
    </div>
    <a class="left carousel-control" href="#carousel-example-generic" role="button" data-slide="prev">
        <span class="glyphicon glyphicon-chevron-left" aria-hidden="true"></span>
        <span class="sr-only">Previous</span>
    </a>
    <a class="right carousel-control" href="#carousel-example-generic" role="button" data-slide="next">
        <span class="glyphicon glyphicon-chevron-right" aria-hidden="true"></span>
        <span class="sr-only">Next</span>
    </a>
    </div>

"""

back = """
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
	  <div class="container">
        <table class="table table-bordered">
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
	  </div>
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
    result+=back
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
            result+=str(y).encode('utf-8')
    curs.close()
    return result

@app.route('/')
def main():
	result = front
	result += """
		<div class="container">
			<div class="starter-template">
				<h1>PSS</h1>
				<p class="lead">설명하는 곳 <br> HTML CSS JS BOOTSTRAP </b>
			</div>
		</div>
	"""
	result += back
	return result

@app.route('/bootstrap')
def bootstrap():
    return render_template('bootstrap.html') 

@app.route('/meal')
def meal():
    result = front
    url = "http://www.pusan.ac.kr/kor/CMS/MenuMgr/menuListOnBuilding.do?"
    soup = BeautifulSoup(init(url), 'html.parser')
    main2 = soup.select_one('#cont > div.menu-wr > div.wauto-wrap > div.is-wauto-box > table.menu-tbl')
    soup_string = str(main2)
    result += """
        <div class="
    """
    result += soup_string
    result += back
    return result


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
