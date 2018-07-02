
from django.conf.urls import url
from django.conf.urls import include

from . import views

# The original tutorial code put the views.hello etc. in quotes.
# This quoted string convention appears to be old and not supported
# any more.  The actual function dotted name is used now.

#urlpatterns = [
#   url(r'^hello/', views.hello, name = 'hello'),
#   url(r'^morning/', views.morning, name = 'morning'),
#   url(r'^aloha/', views.aloha, name = 'aloha'),
#]


# Another tutorial uses path instead of url. This is a django 2
# new thing.  Supposed to be simpler syntax.  Need to play with
# it a lot to figure it out.

from django.urls import path

urlpatterns = [
    path('hello/', views.hello, name='hello'),
    path('aloha/', views.aloha, name='aloha'),
    path('morning/', views.morning, name='morning'),
]

