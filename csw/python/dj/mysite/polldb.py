#!/usr/bin/env python

import os, sys



#
# This block seems to work for getting django initialized
# in a stand alone python script.  There were several
# google hits discussing this, with most of them not helpful.
# Once I found this and it works, I did not look for any
# other solution.
#

if __name__ == "__main__":

    proj_path = "mysite"
    # This is so Django knows where to find stuff.
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "mysite.settings")
    sys.path.append(proj_path)
    
    # This is so my local_settings.py gets loaded.
    os.chdir(proj_path)
    
    # This is so models get loaded.
    from django.core.wsgi import get_wsgi_application
    application = get_wsgi_application()


#  End of "initialization" block



from polls.models import Question, Choice
from django.utils import timezone

qset = Question.objects.all ()

#for e in Question.objects.all():
for e in qset:
    print (e)


while 1:
    print ()
    qtxt = input ("Enter question text: ")
    if qtxt:  
        q = Question(question_text=qtxt, pub_date=timezone.now())
        q.save()
        print ("ID = ", str(q.id))
    else:
        break

print ()

for e in Question.objects.all():
    print (e)


