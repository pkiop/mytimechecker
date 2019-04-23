"""
This script runs the application using a development server.
It contains the definition of routes and views for the application.
"""

from flask import Flask
import json
import pymysql 

app = Flask(__name__)

# Make the WSGI interface available at the top level so wfastcgi can get it.
wsgi_app = app.wsgi_app

conn = pymysql.connect(host='192.168.1.183', user='root', password='aaaa', db='pkiop_planner', charset='utf8')
curs = conn.cursor()


@app.route('/DBget')
def DBget():
    curs.execute("select * from result")
    rows = curs.fetchall()
    result = """
        <style>
         table {
            border: 1px solid #444444;
            border-collapse: collapse;
        }
        th, td {
            border: 1px solid #444444;
            padding: 10px;
            text-align: center;
        }
        </style>
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
        print(x)
        result += "<tr>"
        for y in x:
            print(y)
            result+="<td>"
            result+=str(y)
            result+="</td>"
        result += "</tr>"

    result+="</tbody>"
    result+="</table>"
    return result
    

@app.route('/')
def hello():
    """Renders a sample page."""
    return "Hello World!"

@app.route('/DB')
def db_access():
    return "It's db"

if __name__ == '__main__':
    import os
    HOST = os.environ.get('SERVER_HOST', 'localhost')
    try:
        PORT = int(os.environ.get('SERVER_PORT', '5555'))
    except ValueError:
        PORT = 5555
    app.run(HOST, PORT)
