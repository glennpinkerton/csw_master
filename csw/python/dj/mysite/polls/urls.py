from django.urls import path

from . import views

# Setting the appname variable acts as a namespace.
# In html templates, use 'polls:detail" (for example)
# rather than just 'detail'.   I assume that using
# a variable named app_name for anything else might
# be a problem.

app_name = 'polls'

urlpatterns = [
    path('index/', views.index, name='index'),
    path('hello/', views.hello, name='hello'),
    path('hello3/', views.hello3, name='hello3'),
    path('aloha/', views.aloha, name='aloha'),
    path('morning/', views.morning, name='morning'),

  # In each of these url patterns, question_id is the exact name
  # of the single variable passed to the view function (detail,
  # results, vote)

  #  type in web browser  polls/5/ (as an example) to show the detail of
  #  question with id = 5
    path('<int:question_id>/', views.detail, name='detail'),

  # type /polls/5/results/  for results from question 5
    path('<int:question_id>/results/', views.results, name='results'),

  # type /polls/5/vote/  to vote on question 5
    path('<int:question_id>/vote/', views.vote, name='vote'),

]  # end of url pattern constants
