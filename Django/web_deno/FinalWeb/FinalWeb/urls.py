"""
URL configuration for FinalWeb project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/4.2/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.urls import path
from web.views import department, employee, prettynum, admin, account, contract

"""
    URL：
    URL 是用来唯一标识 Web 资源（比如网页、图片、文件等）的字符串。
    它通常由协议名、服务器地址、路径和查询参数组成，用于 Web 浏览器或者 Web 服务器定位和获取特定的资源。
    在 Django 中，URL 也被用作路由规则，它将用户发送的请求映射到相应的视图函数上。
"""

"""
    视图函数：
    图函数是 Django Web 应用程序中处理用户请求的 Python 函数。
    接收一个 HTTP 请求作为参数，并返回一个 HTTP 响应。
    视图函数负责处理用户请求、执行业务逻辑、从数据库中获取数据、渲染模板等任务。
    在 Django 中，视图函数通常被定义在 views.py 文件中
    可以通过 URL 路由规则来将特定的 URL 请求映射到相应的视图函数上。
"""

"""
    例子：
    当用户访问网站的某个页面时，浏览器会发送一个 HTTP 请求到服务器，并包含了用户请求的 URL 路径。
    服务器根据这个 URL 路径通过 URL 配置文件（urls.py）找到相应的路由规则，然后将请求转发给对应的视图函数来处理。
    视图函数处理完请求后，可以返回一个 HTML 页面作为响应，也可以返回一个重定向，或者其他类型的响应。
    URL 和视图函数是 Django Web 应用程序中实现页面路由和业务逻辑的重要组成部分，它们共同构成了 Web 应用程序的核心架构。
"""
urlpatterns = [
    # 部门列表
    path('department/list/', department.department_list),
    # 添加部门
    path('department/add/', department.department_add),
    # 删除部门
    path('department/delete/', department.department_delete),
    # 编辑部门(正则表达式)
    path('department/<int:nid>/edit/', department.department_edit),

    # 用户列表
    path('user/list/', employee.user_list),
    # 添加用户
    path('user/add/', employee.user_add),
    # 删除用户
    path('user/delete/', employee.user_delete),
    # 编辑用户
    path('user/<int:nid>/edit/', employee.user_edit),

    # 靓号列表
    path('prettynum/list/', prettynum.prettynum_list),
    # 添加靓号
    path('prettynum/add/', prettynum.prettynum_add),
    # 删除靓号
    path('prettynum/delete/', prettynum.prettynum_delete),
    # 编辑靓号
    path('prettynum/<int:nid>/edit/', prettynum.prettynum_edit),

    # 管理员列表
    path('admin/list/', admin.admin_list),
    # 添加管理员
    path('admin/add/', admin.admin_add),
    # 编辑管理员
    path('admin/<int:nid>/edit/', admin.admin_edit),
    # 删除管理员
    path('admin/<int:nid>/delete/', admin.admin_delete),
    # 重置密码
    path('admin/<int:nid>/reset/', admin.admin_reset),

    # 登录
    path('login/', account.login),
    # 注销
    path('logout/', account.logout),

    # 合同列表
    path('contract/list/', contract.contract_list),
    # 添加合同
    path('contract/add/', contract.contract_add),
    # 编辑合同
    path('contract/<int:nid>/edit/', contract.contract_edit),
    # 删除合同
    path('contract/<int:nid>/delete/', contract.contract_delete),
]
