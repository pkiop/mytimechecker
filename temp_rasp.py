# -*- coding: utf-8 -*-

#server, db
from flask import Flask, render_template, redirect, url_for, request
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
import os



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

localhost = '164.125.63.208:5555'
dbhost = '192.168.1.182'

#db는 연결해두고 커서만 끄자
#conn = pymysql.connect(host=dbhost , user='bitelab_cl',
#                       password='1111', db='bitelab', charset='utf8')


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

	<title>비이츠랩의 비밀공간</title>

	<!-- Bootstrap core CSS -->
	<link href="/static/css/bootstrap.min.css" rel="stylesheet">

	<!-- Custom styles for this template -->
	<link href="/static/css/starter-template.css" rel="stylesheet">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->
    <meta name="description" content="">
    <meta name="author" content="">
    <link rel="icon" href="http://bootstrapk.com/favicon.ico">

    <!-- Bootstrap core CSS -->
    <link href="./static/css/bootstrap.min.css" rel="stylesheet">
    <!-- Bootstrap theme -->
    <link href="./static/css/bootstrap-theme.min.css" rel="stylesheet">

    <!-- Custom styles for this template -->
    <link href="./static/css/theme.css" rel="stylesheet">

    <!-- Just for debugging purposes. Don't actually copy these 2 lines! -->
    <!--[if lt IE 9]><script src="../../assets/js/ie8-responsive-file-warning.js"></script><![endif]-->
    <script src="./static/js/ie-emulation-modes-warning.js.다운로드"></script>

    <!-- HTML5 shim and Respond.js for IE8 support of HTML5 elements and media queries -->
    <!--[if lt IE 9]>
      <script src="https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js"></script>
      <script src="https://oss.maxcdn.com/respond/1.4.2/respond.min.js"></script>
    <![endif]-->
</head>
"""

active_header = """
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
				<a class="navbar-brand"	href="http://""" + localhost + """">비이츠랩의 비밀공간</a>
			</div>
			<div id="navbar" class="collapse navbar-collapse">
				<ul class="nav navbar-nav">
					<li><a href="http://""" + localhost + """/">main</a></li>
					<li><a href="http://""" + localhost + """/meal">식단</a></li>
                    <li><a href="http://""" + localhost + """/goodwrite">좋은 글</a></li>
                    <li><a href="http://""" + localhost + """/guestbook">방명록</a></li>
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
    result += active_header.replace('<li><a href="http://' + localhost + '/">main</a></li>','<li class="active"><a href="http://' + localhost + '/">main</a></li>')
    result += """
        <br><br><br><br>
        <div class="container">
	        <img width="300px" height="300px" src="https://raw.githubusercontent.com/pkiop/mytimechecker/master/image/godfather.jpg" class="img-responsive center-block img-circle"/>
        </div>
		<div class="container">
			<div class="starter-template">
				<h1>비이츠랩</h1>
				<p class="lead"><br> </b>
			</div>
		</div>
<footer class="footer">
<div class="containter">
<div id="carouselExampleSlidesOnly" class="carousel slide fixed-bottom" data-ride="carousel">
    <div class="carousel-inner">
        <div class="item active">
        <a href="https://ee.pusan.ac.kr"><img class="center-block" src="https://raw.githubusercontent.com/pkiop/mytimechecker/master/image/eelogo.png" alt="First slide"></a>
        </div>
        <div class="item">
        <a href="https://www.pusan.ac.kr"><img class="center-block" src="https://raw.githubusercontent.com/pkiop/mytimechecker/master/image/pusanlogo.png" alt="Second slide"></a>
        </div>
    </div>
</div>
</div>
</footer>
	"""
    result += back
    return result

@app.route('/bootstrap')
def bootstrap():
    return render_template('bootstrap.html') 

@app.route('/meal')
def meal():
    result = front
    result += active_header.replace('<li><a href="http://' + localhost + '/meal">식단</a></li>','<li class="active"><a href="http://' + localhost + '/meal">식단</a></li>')
    result += """
        <style>
        ul{
            list-style:none;
        }
        @media (max-width: 1000px){
            .container#red{
                width:1000px;
            }
        }
        @media (max-width: 1000px){
            .container#sub{
                width:600px;
            }
        }
        </style>
    """


    #url = "http://www.pusan.ac.kr/kor/CMS/MenuMgr/menuListOnWeekly.do?" 임시로 어제것 사용하기 위함
    url = "http://www.pusan.ac.kr/kor/CMS/MenuMgr/menuListOnWeekly.do?mCode=#childTab_tmp"
    result += """
    <div class="container">
    <h2 class="text-center">\u2764\uFE0F오늘의 행복\u2764\uFE0F</h2>
    </div>
    <div class="container" id="sub">
        <h3>금정회관 2층</h3>
        <table class="table table-striped">
            <thead>
                <tr>
                    <th style="width: 20%"></th>
                    <th style="width: 0%"></th>
                    <th style="width: 40%" class="text-center">중식</th>
                    <th style="width: 40%" class="text-center">석식</th>
                </tr>
            </thead>
            <tbody>
    """
    soup = BeautifulSoup(init(url), 'html.parser')
    main3 = soup.select_one('#cont > div.menu-wr > div.is-wauto-box.menu-tbl-wr > table > tbody > tr:nth-child(1)')
    soup_string = str(main3)

    soup_string = soup_string.replace('<h3 class="menu-tit03">정식-4,000원</h3>','')
    soup_string = soup_string.replace('<h3 class="menu-tit03">정식-3,000원</h3>','')
    soup_string = soup_string.replace('<h3 class="menu-tit03">일품1-3,000원</h3>','')

    result += soup_string
    result += "</tbody></table></div>"





    #url = "http://www.pusan.ac.kr/kor/CMS/MenuMgr/menuListOnWeekly.do?" 임시로 어제것 사용하기 위함
    url = "http://www.pusan.ac.kr/kor/CMS/MenuMgr/menuListOnWeekly.do?mCode=#childTab_tmp"
    result += """
    <div class="container" id="sub">
        <h3>금정회관 1층</h3>
        <table class="table table-striped">
            <thead>
                <tr>
                    <th style="width: 20%"></th>
                    <th style="width: 26%" class="text-center">조식</th>
                    <th style="width: 26%" class="text-center">중식</th>
                    <th style="width: 26%" class="text-center">석식</th>
                </tr>
            </thead>
            <tbody>
    """
    soup = BeautifulSoup(init(url), 'html.parser')
    main3 = soup.select_one('#cont > div.menu-wr > div.is-wauto-box.menu-tbl-wr > table > tbody > tr:nth-child(2)')
    soup_string = str(main3)

    soup_string = soup_string.replace('<h3 class="menu-tit03">정식-4,000원</h3>','')
    soup_string = soup_string.replace('<h3 class="menu-tit03">정식-3,000원</h3>','<h4 class="text-center">정식</h4>')
    soup_string = soup_string.replace('<h3 class="menu-tit03">일품1-3,000원</h3>','<h4 class="text-center">일품1</h4>')
    soup_string = soup_string.replace('<h3 class="menu-tit03">일품2-3,000원</h3>','<h4 class="text-center">일품2</h4>')

    result += soup_string
    result += "</tbody></table></div>"





    result +=        """
     <div class="container" id="red">
        <h3>금정회관 2층</h3>
        """

    url = "http://www.pusan.ac.kr/kor/CMS/MenuMgr/menuListOnBuilding.do?"
    soup = BeautifulSoup(init(url), 'html.parser')
    main2 = soup.select_one('#cont > div.menu-wr > div.wauto-wrap > div.is-wauto-box > table.menu-tbl')
    soup_string = str(main2)
    soup_string = soup_string.replace('조식','')
    soup_string = soup_string.replace('<th scope="col">','<th style="width: 19%">')
    soup_string = soup_string.replace('<th style="width: 19%">구분','<th style="width: 3%">')
    soup_string = soup_string.replace('<th style="width: 19%">','<th style="width:19%" class="text-center">')
    soup_string = soup_string.replace('<table class="menu-tbl type-day"', '<table class="table table-striped"')
    soup_string = soup_string.replace('<caption><span class="blind">캠퍼스별 식단메뉴에 대한 안내제공</span></caption>', '')
    soup_string = soup_string.replace('<h3 class="menu-tit01">정식-4,000원</h3>','')
    #soup_string = soup_string.replace('<div class="day">토</div>','')
    #soup_string = soup_string.replace('<div class="date">2019.05.04</div>','')
    result += soup_string
    result += """
        </div>
    """
    result



  
    result += back
    return result

@app.route('/guestbook', methods=['POST','GET'])
def guestbook():
#    conn = pymysql.connect(host='192.168.1.183', user='root', password='1111', db='bitelab', charset='utf8')
#    curs = conn.cursor()
#    curs.execute("select * from guestbook")
#    rows = curs.fetchall()
    print('do connect')
    conn = pymysql.connect(host=dbhost , user='bitelab_cl', password='1111', db='bitelab', charset='utf8')
    print('connect success')
    curs = conn.cursor()
    if request.method == 'POST':
        writer = request.form['writer']
        contents = request.form['contents']
        
        query = "INSERT INTO guestbook (name, text) VALUES ('" + writer + "', '" + contents + "')"
        returnvalue= curs.execute(query)
        conn.commit()
    result = front
    result += active_header.replace('<li><a href="http://' + localhost + '/guestbook">방명록</a></li>','<li class="active"><a href="http://' + localhost + '/guestbook">방명록</a></li>')
    result += """
    <style>
       
        @media (max-width: 800px){
            .container#sub{
                width:800px;
            }
        }
        </style>
<div class="container">
<table class="table table-bordered">
    <thead>
    </thead>
    <tbody>
        <form action="http://""" + localhost + """/guestbook" accept-charset="utf-8" method="post">
            <tr>
                <th>작성자: </th>
                <td><input type="text" placeholder="작성자을 입력하세요. " name="writer" class="form-control" /></td>
            </tr>
            <tr>
                <th>내용: </th>
                <td><textarea cols="10" placeholder="내용을 입력하세요. " name="contents" class="form-control" ></textarea></td>
            </tr>
            <tr>
                <td colspan="2">
                    <input type="submit" class="btn btn-primary" value="등록" />
                </td>
            </tr>
        </form>
    </tbody>
</table>
</div>

<div class="container" id="sub">
<table class="table table-striped">
<thead>
    <th style="width: 8%">번호</th>
    <th style="width: 65%">내용</th>
    <th style="width: 10%">작성자</th>
    <th style="width: 17%">작성일자</th>
</thead>
<tbody>
"""
    curs.execute("select * from guestbook")
    dbstring = curs.fetchall()
    for x in dbstring:
        result += "<tr>"
        for y in x : 
            result+="<td>"
            result+=str(y)
            result+="</td>"
        result += "</tr>"

    result += """
 </tbody>
    </div>
    
     """

    result+=back
    curs.close()
    conn.close()
    return result








@app.route('/goodwrite')
def goodwritecompile():

    filename = '컴파일과 빌드'
    fileroute = '/goodwrite/compile'

    result = front
    result += active_header.replace('<li><a href="http://' + localhost + '/goodwrite">좋은 글</a></li>','<li class="active"><a href="http://' + localhost + '/goodwrite">좋은 글</a></li>')
    result += '<div class="container">'
    result += '<h3>' + filename + '</h3>' + '</div><br><br>'
    result += '<div class="container">'
    f = open("./goodwrite/compile.html", "r", encoding='utf8')
    data = f.read()
     
    data = data.replace('compile_files','https://raw.githubusercontent.com/pkiop/mytimechecker/master' + fileroute + '_files')
    data = data.replace('<a name="7616" />','')
    data = data.replace('/static','../static')
    f.close()
    result += data
    result += '</div>'
    result += back
    return result

@app.route('/DB')
def db_access():
    return "It's db"

if __name__ == '__main__':
    os.path.join(app.instance_path, '/')
    HOST = os.environ.get('SERVER_HOST', '0.0.0.0')
    try:
        PORT = int(os.environ.get('SERVER_PORT', '5555'))
    except ValueError:
        PORT = 5555
    app.run(HOST, PORT, threaded=True) # Threaded=True => multiple plot이 가능해짐 
