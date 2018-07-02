from django.shortcuts import render

# Create your views here.

from django.http import HttpResponse

# To show this view in a web browser, type localhost:8000/blogs
# for the browser url.

def hello(request):
   text = """<h1>welcome to my app !</h1>"""
   return HttpResponse(text)

# To show this view in a web browser, type localhost:8000/blogs/aloha
# for the browser url.

def aloha(request):
   text = """<h2>Aloha my friend !</h2>"""
   return HttpResponse(text)



def morning(request):
   text = """<h2>Top o the morning to ya Lad</h2>"""
   return HttpResponse(text)
