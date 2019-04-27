#-*- coding: utf-8 -*-

import sys
import io
from bs4 import BeautifulSoup
import requests
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
import time
import json
#sys.stdout.reconfigure(encoding='utf-8')
#sys.stdout = io.TextIOWrapper(sys.stdout.detach(), encoding = 'utf-8')
#sys.stderr = io.TextIOWrapper(sys.stderr.detach(), encoding = 'utf-8')

chrome_options = Options()
chrome_options.add_argument("--headless")
driver = webdriver.Chrome(chrome_options=chrome_options, executable_path="/usr/lib/chromium-browser/chromedriver")#"home/pi/Project/mytimechecker/crawler/chromedriver")
#driver = webdriver.Chrome("/home/pi/Project/mytimechecker/crawler/chromedriver")
driver.set_page_load_timeout(10)

def init(url):
    driver.get(url)
    driver.implicitly_wait(0.1)
    return driver.page_source

url = "http://www.pusan.ac.kr/kor/CMS/MenuMgr/menuListOnBuilding.do?"
soup = BeautifulSoup(init(url), 'html.parser')
file = open('meal.html','w')

#print(soup)
main2 = soup.select_one('#cont > div.menu-wr > div.wauto-wrap > div.is-wauto-box > table.menu-tbl')
soup_string = str(main2)
file.write(soup_string)

driver.close()