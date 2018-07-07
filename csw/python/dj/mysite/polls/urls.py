from django.urls import path

from . import views

urlpatterns = [
    path('index/', views.index, name='index'),
    path('hello/', views.hello, name='hello'),
    path('hello3/', views.hello3, name='hello3'),
    path('aloha/', views.aloha, name='aloha'),
    path('morning/', views.morning, name='morning'),
]
