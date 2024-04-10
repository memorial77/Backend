from django.shortcuts import render, redirect
from web import models


# 部门列表
def department_list(request):
    # 从数据库中查询获取所有部门列表
    # Django ORM - Object - RelationalMapping
    # 用于在面向对象的编程语言（如Python）和关系型数据库（如MySQL）之间建立映射关系
    query_set = models.Department.objects.all()

    """
        Django中，render是一个快捷函数，用于将渲染后的HTML页面作为HTTP响应返回给客户端
        通常用于在视图函数中将模板与上下文数据结合起来渲染成HTML页面
        将名为 'department_list.html' 的模板与一个名为 query_set 的上下文数据结合起来渲染成 HTML 页面
        并将渲染后的页面作为 HTTP 响应返回给客户端
        在模板中，你可以通过变量名 query_set 来访问传递进来的 query_set 对象
    """
    return render(request, 'department_list.html', {'query_set': query_set})


# 添加部门
def department_add(request):
    # 处理添加部门请求，GET请求返回添加部门页面，POST请求处理表单提交并添加部门
    if request.method == 'GET':
        """
            为了让视图函数能够访问 request 对象，并将生成的响应返回给客户端，render 函数需要接收 request 参数
        """
        return render(request, 'department_add.html')
    else:
        # 处理表单提交并添加部门
        title = request.POST.get('title')
        models.Department.objects.create(title=title)
        # 添加完成后重定向回部门列表页面查看添加结果
        return redirect('/department/list')


# 删除部门
def department_delete(request):
    # 处理删除部门请求，获取部门ID并删除对应部门
    nid = request.GET.get('nid')
    models.Department.objects.filter(id=nid).delete()
    # 删除完成后重定向回部门列表页面查看添加结果
    return redirect('/department/list')


# 编辑部门函数
def department_edit(request, nid):
    # 处理编辑部门请求，GET 请求返回编辑部门页面，POST 请求处理表单提交并更新部门信息。
    if request.method == 'GET':
        # 获取部门标题
        row_object = models.Department.objects.filter(id=nid).first()
        return render(request, 'department_edit.html', {'title': row_object.title})
    else:
        # 处理表单提交并更新部门信息
        new_title = request.POST.get('new_title')
        models.Department.objects.filter(id=nid).update(title=new_title)
        return redirect('/department/list')
