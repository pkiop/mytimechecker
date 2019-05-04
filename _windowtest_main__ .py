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
driver = webdriver.Chrome(chrome_options=chrome_options, executable_path="./crawler/chromedriver.exe")#"home/pi/Project/mytimechecker/crawler/chromedriver")
#driver = webdriver.Chrome("/home/pi/Project/mytimech/ecker/crawler/chromedriver")
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

	<title>비이츠랩의  비밀공간</title>

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
					href="http://164.125.63.208:5555">비이츠랩의 비밀공간</a>
			</div>
			<div id="navbar" class="collapse navbar-collapse">
				<ul class="nav navbar-nav">
					<li class="active"><a href="http://164.125.63.208:5555/">main</a></li>
					<li><a href="http://164.125.63.208:5555/meal">식단</a></li>
					<li><a href="https://bitelab.pusan.ac.kr/ccslab/index.do">Bitelab</a></li>
				</ul>
			</div>
			<!--/.nav-collapse -->
		</div>
	</nav>

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
			<img width="300px" height="300px" src="https://raw.githubusercontent.com/pkiop/mytimechecker/master/image/godfather.jpg" class="img-responsive center-block img-circle"/>
		</div>
		<div class="container">
			<div class="starter-template">
				<h1>비이츠랩</h1>
				<p class="lead"><br> </b>
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

    soup_string = soup_string.replace('menu-tbl type-day','table table-striped')		
    soup_string = soup_string.replace('<td>', '<td class="col-md-15">')
    soup_string = soup_string.replace('<h3 class="menu-tit01">정식-4,000원</h3>', '')
    soup_string = soup_string.replace('<span class="blind">캠퍼스별 식단메뉴에 대한 안내제공</span>','')
    soup_string = soup_string.replace('h3','p')
    soup_string = soup_string.replace('조식','')
    result += """
		<div class="container">
			<div class="starter-template">
	"""
    result += soup_string
    result += """
			</div>
        </div>

	"""
    result += back
    return result


@app.route('/meal_oneday')
def meal_oneday():
    result = front
    url = "http://www.pusan.ac.kr/kor/CMS/MenuMgr/menuListOnWeekly.do"
    soup = BeautifulSoup(init(url), 'html.parser')
    main2 = soup.select_one('#cont > div.menu-wr > div.wauto-wrap > div.is-wauto-box.menu-tbl-wr > table > tbody')
    soup_string = str(main2)

    result += """
		<div class="container">
			<div class="starter-template">
	"""
    result += soup_string
    result += """
			</div>
        </div>

	"""
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
