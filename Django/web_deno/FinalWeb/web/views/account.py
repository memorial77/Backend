from django.shortcuts import render, redirect
from django import forms

from web import models
from web.utils.bootstrap import BootStrapForm
from web.utils.encrypt import md5


class LoginForm(BootStrapForm):
    username = forms.CharField(
        label="",
        widget=forms.TextInput,
        required=True
    )
    password = forms.CharField(
        label="",
        widget=forms.PasswordInput(render_value=True),
        required=True
    )

    def clean_password(self):
        pwd = self.cleaned_data.get("password")
        return md5(pwd)


# 登录
def login(request):
    if request.method == "GET":
        form = LoginForm()
        return render(request, 'login.html', {'form': form})

    form = LoginForm(data=request.POST)
    if form.is_valid():
        # 去数据库校验用户名和密码是否正确，获取用户对象/None
        admin_object = models.Admin.objects.filter(**form.cleaned_data).first()
        if not admin_object:
            form.add_error("password", "Incorrect username or password.")
            return render(request, 'login.html', {'form': form})

        # 用户名和密码正确
        # 网站生成随机字符串 写到用户浏览器的cookie中再写入到session中
        request.session["info"] = {'id': admin_object.id, 'name': admin_object.username}
        # session   保存7天
        request.session.set_expiry(60 * 60 * 24 * 7)

        return redirect("/admin/list/")

    return render(request, 'login.html', {'form': form})


def logout(request):
    request.session.clear()
    return redirect('/login/')
