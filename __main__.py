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

conn = pymysql.connect(host='localhost', user='root', password='1111', db='pkiop_planner', charset='utf8')
curs = conn.cursor()


@app.route('/DBget')
def DBget():
    curs.execute("select * from result5")
    rows = curs.fetchall()
    result = """
        <table>
         <thead>
            <tr>
            <th>ID</th>
            <th>DATE</th>
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
        print("x임")
        print(x)
        result += "<tr>"
        for y in x:
            print("y임")
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
