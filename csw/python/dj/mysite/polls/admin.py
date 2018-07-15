
#  This file is used to register model classes for the built
#  in django admin interface.  To run admin for this example
#  site.    localhost:8000/admin   Obviously the server needs
#  to be running as well.

from django.contrib import admin

# Register your models here.

from .models import Question

admin.site.register(Question)
