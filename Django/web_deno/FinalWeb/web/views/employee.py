from django.shortcuts import render, redirect
from django.utils.safestring import mark_safe

from web import models
from web.utils.form import UserModelForm
from web.utils.paginate import paginate_query_set


# def user_list(request):
#     data_dict = {}
#     search_data = request.GET.get('search', "")
#     if search_data:
#         data_dict["name__contains"] = search_data
#
#     # 默认搜索的范围为第一页，page不传值或者首次访问该页面GET请求时默page认为1
#     page = int(request.GET.get('page', 1))
#     # 每页显示的数据条数
#     page_size = 10
#     start = (page - 1) * page_size
#     end = page * page_size
#
#     # 根据数据总条数计算出需要展示的页码
#     total_count = models.Employee.objects.filter(**data_dict).count()
#     page_count, div = divmod(total_count, page_size)
#     # 存在余数则总页码增加1
#     if div:
#         page_count += 1
#
#     # 设置始终显示的页码数量
#     displayed_pages = 4
#     # 计算起始页码
#     start_page = max(1, min(page_count - displayed_pages + 1, page - (displayed_pages // 2)))
#     # 计算结束页码
#     end_page = min(page_count, start_page + displayed_pages - 1)
#
#     page_str_list = []
#     # 使用 range() 函数生成页码列表
#     for i in range(start_page, end_page + 1):
#         if i == page:
#             # 当前页码加上 active 类高亮
#             page_str_list.append('<li class="active"><a href="?page={}">{}</a></li>'.format(i, i))
#         else:
#             if search_data:
#                 page_str_list.append('<li><a href="?page={}&search={}">{}</a></li>'.format(i, search_data, i))
#             else:
#                 page_str_list.append('<li><a href="?page={}">{}</a></li>'.format(i, i))
#
#     # 将列表转换为字符串
#     page_string = mark_safe("".join(page_str_list))
#
#     # 根据字典内的内容在数据库中进行检索并根据level排序，[start: end]表示搜索范围即页数
#     # 此处使用-level即按照level从大到小进行检索
#     query_set = models.Employee.objects.filter(**data_dict)[start: end]
#
#     # 需要渲染进HTML文件的数据
#     context = {
#         "search_data": search_data,
#         "query_set": query_set,
#         "page_string": page_string,
#         "page_count": page_count,
#     }
#     return render(request, 'user_list.html', context)

def user_list(request):
    query_set = models.Employee.objects.all()
    context = paginate_query_set(request, query_set, 'name')
    return render(request, 'user_list.html', context)


"""
    ModelForm 是 Django 中的一个类，用于创建基于模型（Model）的表单。
    提供了一种简单的方式来创建 HTML 表单，并与 Django 模型（通常是数据库中的数据表）进行关联。
    使用 ModelForm，可以轻松地创建一个表单，该表单与指定的 Django 模型相关联，
    并且会自动处理表单字段与模型字段之间的映射关系。
    这意味着无需手动为每个表单字段编写代码，而是可以直接利用模型中定义的字段来创建表单。
    只需定义一个继承自 forms.ModelForm 的子类，并指定 Meta 类的 model 属性为你要关联的模型即可。
    ModelForm 会自动根据模型的字段生成相应的表单字段，并在表单验证和处理过程中与模型进行数据交互。
"""

"""
    使用 ModelForm 的主要优点包括：
    1.简化了表单的创建过程，节省了编写重复代码的时间和精力。
    2.与模型字段的自动映射使得表单与模型之间的数据交互更加方便和安全。
    3.提高了代码的可维护性，当模型结构发生变化时，相关的表单字段也会相应更新。
    ModelForm 是 Django 中用于创建与模型相关联的表单的工具
    简化了表单的创建和处理过程，并提供了与模型之间数据交互的便利方法。
"""


# 添加用户(ModelForm版本)
def user_add(request):
    if request.method == 'GET':
        """
            当请求方法为 GET 时，创建一个 UserModelForm 实例并将其传递给模板，以便在页面上显示一个空的表单。
        """
        form = UserModelForm()
        return render(request, 'user_add.html', {'form': form})
    else:
        """
            当请求方法为 POST 时，创建一个带有请求数据的 UserModelForm 实例，并检查表单数据的有效性。
            如果表单数据有效，则将数据保存至数据库，并重定向到员工列表页面查看数据列表
            如果表单数据无效，则重新渲染带有错误信息的表单页面，让用户重新填写。
        """
        form = UserModelForm(data=request.POST)
        if form.is_valid():
            # 数据合法则保存至数据库
            form.save()
            return redirect('/user/list/')
        else:
            return render(request, 'user_add.html', {'form': form})


# 删除用户
def user_delete(request):
    # 从GET请求中获取参数nid并删除对应员工
    nid = request.GET.get('nid')
    models.Employee.objects.filter(id=nid).delete()
    return redirect('/user/list/')


# 编辑用户
def user_edit(request, nid):
    # 根据传入的nid获取对应的员工对象
    row_object = models.Employee.objects.filter(id=nid).first()

    # GET请求表示用户请求编辑表单页面
    if request.method == 'GET':
        # 根据获取的员工对象实例化一个用户表单用于展示数据
        form = UserModelForm(instance=row_object)
        return render(request, 'user_edit.html', {'form': form})
    else:
        # POST请求表示用户提交了编辑表单
        # 从POST请求中获取表单数据，同时传入获取的员工对象，以便表单更新数据
        form = UserModelForm(data=request.POST, instance=row_object)
        # 数据合法则保存至数据库
        if form.is_valid():
            form.save()
            return redirect('/user/list/')
        else:
            # 如果表单数据无效，重新渲染编辑页面，并将表单对象传递给模板
            # 重新传给模板是为了能够保留显示之前编辑过的信息
            return render(request, 'user_edit.html', {'form': form})
