from django.shortcuts import render, redirect

from web import models
from web.utils.bootstrap import BootStrapModelForm
from web.utils.paginate import paginate_query_set


class ContractEditModelForm(BootStrapModelForm):
    class Meta:
        # 关联的模型为 Contract
        model = models.Contract
        fields = ['department', 'start_date', 'end_date', 'content']


# def contract_list(request):
#     data_dict = {}
#     search_data = request.GET.get('search', "")
#     if search_data:
#         data_dict["content__contains"] = search_data
#
#     # 默认搜索的范围为第一页，page不传值或者首次访问该页面GET请求时默page认为1
#     page = int(request.GET.get('page', 1))
#     # 每页显示的数据条数
#     page_size = 10
#     start = (page - 1) * page_size
#     end = page * page_size
#
#     # 根据数据总条数计算出需要展示的页码
#     total_count = models.Contract.objects.filter(**data_dict).count()
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
#     query_set = models.Contract.objects.filter(**data_dict)[start: end]
#
#     # 需要渲染进HTML文件的数据
#     context = {
#         "search_data": search_data,
#         "query_set": query_set,
#         "page_string": page_string,
#         "page_count": page_count,
#     }
#     return render(request, 'contract_list.html', context)

def contract_list(request):
    query_set = models.Contract.objects.all()
    context = paginate_query_set(request, query_set, 'content')
    return render(request, 'contract_list.html', context)


def contract_delete(request, nid):
    models.Contract.objects.filter(id=nid).delete()
    return redirect('/contract/list/')


def contract_edit(request, nid):
    # 根据传入的nid获取对应的员工对象
    row_object = models.Contract.objects.filter(id=nid).first()

    # GET请求表示用户请求编辑表单页面
    if request.method == 'GET':
        # 根据获取的员工对象实例化一个用户表单用于展示数据
        form = ContractEditModelForm(instance=row_object)
        return render(request, 'contract_edit.html', {'form': form})
    else:
        # POST请求表示用户提交了编辑表单
        # 从POST请求中获取表单数据，同时传入获取的员工对象，以便表单更新数据
        form = ContractEditModelForm(data=request.POST, instance=row_object)
        # 数据合法则保存至数据库
        if form.is_valid():
            form.save()
            return redirect('/contract/list/')
        else:
            # 如果表单数据无效，重新渲染编辑页面，并将表单对象传递给模板
            # 重新传给模板是为了能够保留显示之前编辑过的信息
            return render(request, 'contract_edit.html', {'form': form})


class ContractAddModelForm(BootStrapModelForm):
    class Meta:
        # 关联的模型为 Contract
        model = models.Contract
        fields = ['employee', 'department', 'start_date', 'end_date', 'content']


def contract_add(request):
    if request.method == 'GET':
        form = ContractAddModelForm()
        return render(request, 'contract_add.html', {'form': form})
    else:
        form = ContractAddModelForm(data=request.POST)
        if form.is_valid():
            # 数据合法则保存至数据库
            form.save()
            return redirect('/contract/list/')
        else:
            return render(request, 'contract_add.html', {'form': form})
