from django.shortcuts import render, redirect
from django.core.validators import RegexValidator
from django.core.exceptions import ValidationError
from web import models
from django import forms

from web.utils.bootstrap import BootStrapModelForm
from web.utils.encrypt import md5

"""
    UserModelForm 继承自 forms.ModelForm，这是 Django 提供的一个便捷的表单类，可以基于模型自动生成表单字段。
"""


class UserModelForm(forms.ModelForm):
    # 定义用户表单
    name = forms.CharField(min_length=3, max_length=20)

    """
        - 在Django的ModelForm中，Meta类用于提供关于模型的额外信息
        - fields属性用于指定在表单中包含哪些模型字段
    """

    class Meta:
        model = models.Employee
        fields = ['name', 'password', 'age', 'salary', 'depart', 'gender']

    """
        - 通过遍历表单的每个字段 为每个字段的widget添加了一个class属性为"form-control"以便应用Bootstrap样式
    """

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        for name, field in self.fields.items():
            field.widget.attrs = {"class": "form-control"}


class PrettyNumModelForm(forms.ModelForm):
    mobile = forms.CharField(
        label="Mobile",
        validators=[RegexValidator(r'^1[3-9]\d{9}$', 'Invalid phone number format.'), ],
    )

    class Meta:
        # 关联的模型为 PrettyNum
        model = models.PrettyNum
        # 表单包含的字段:包括手机号、价格、等级和状态
        fields = ['mobile', 'price', 'level', 'status']

    """
        Django 表单类中的一个特殊方法，用于对表单字段进行自定义验证
        自定义验证方法的执行顺序是在表单对象的is_valid()方法被调用时进行的
        1.Django会执行字段的默认验证器。
        这些验证器通常是在字段的定义中指定的 例如在CharField中指定的max_length
        2.Django会按照字段在表单类中的顺序依次执行各个字段的自定义验证方法。
        3.如果有字段验证失败 表单的 is_valid()方法返回False 并将错误信息存储在表单对象的errors属性中。
    """

    # 验证手机号是否已存在
    def clean_mobile(self):
        txt_mobile = self.cleaned_data["mobile"]
        exists = models.PrettyNum.objects.filter(mobile=txt_mobile).exists()
        if exists:
            raise ValidationError("Phone number already exists.")
        else:
            # 手机号不重复返回验证后的手机号
            return txt_mobile

    # 初始化方法
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        for name, field in self.fields.items():
            field.widget.attrs = {"class": "form-control"}


class PrettyNumEditModelForm(forms.ModelForm):
    mobile = forms.CharField(
        label="Mobile",
        validators=[RegexValidator(r'^1[3-9]\d{9}$', 'Invalid phone number format.'), ],
    )

    class Meta:
        model = models.PrettyNum
        fields = ['mobile', 'price', 'level', 'status']

    # 验证手机号是否已存在，排除当前编辑行
    def clean_mobile(self):
        txt_mobile = self.cleaned_data["mobile"]
        exists = models.PrettyNum.objects.exclude(id=self.instance.pk).filter(mobile=txt_mobile).exists()
        if exists:
            raise ValidationError("Phone number already exists.")
        return txt_mobile

    # 初始化方法
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        for name, field in self.fields.items():
            field.widget.attrs = {"class": "form-control"}


class AdminModelForm(BootStrapModelForm):
    # 定义管理员表单
    confirm_password = forms.CharField(label="Confirm Password",
                                       widget=forms.PasswordInput)

    class Meta:
        model = models.Admin
        fields = ['username', 'password', 'confirm_password']
        widgets = {
            "password": forms.PasswordInput
        }

    # 验证密码是否一致
    def clean_password(self):
        password = self.cleaned_data.get("password")
        return md5(password)

    # 验证确认密码是否一致
    def clean_confirm_password(self):
        password = self.cleaned_data.get("password")
        confirm_password = md5(self.cleaned_data.get("confirm_password"))
        if password != confirm_password:
            raise ValidationError("Not same")
        return confirm_password


class AdminEditModelForm(BootStrapModelForm):
    # 定义编辑管理员表单
    class Meta:
        model = models.Admin
        fields = ['username']


class AdminResetModelForm(BootStrapModelForm):
    # 定义重置管理员密码表单
    confirm_password = forms.CharField(label="Confirm Password",
                                       widget=forms.PasswordInput)

    class Meta:
        model = models.Admin
        fields = ['password', 'confirm_password']
        widgets = {
            "password": forms.PasswordInput
        }

    # 验证新密码是否与旧密码相同
    def clean_password(self):
        password = self.cleaned_data.get("password")
        md5_new_password = md5(password)
        exists = models.Admin.objects.filter(id=self.instance.pk, password=md5_new_password).exists()
        if exists:
            raise ValidationError("The password cannot be the same as the previous one.")
        return md5(password)

    # 验证确认密码是否一致
    def clean_confirm_password(self):
        password = self.cleaned_data.get("password")
        confirm_password = md5(self.cleaned_data.get("confirm_password"))
        if password != confirm_password:
            raise ValidationError("Not same")
        return confirm_password

