from django.shortcuts import render, redirect
from web import models
from django.utils.safestring import mark_safe
from web.utils.form import PrettyNumModelForm, PrettyNumEditModelForm
from web.utils.paginate import paginate_query_set


# 靓号列表
# def prettynum_list(request):
#     data_dict = {}
#     search_data = request.GET.get('search', "")
#     if search_data:
#         data_dict["mobile__contains"] = search_data
#
#     # 默认搜索的范围为第一页，page不传值或者首次访问该页面GET请求时默page认为1
#     page = int(request.GET.get('page', 1))
#     # 每页显示的数据条数
#     page_size = 10
#     start = (page - 1) * page_size
#     end = page * page_size
#
#     # 根据数据总条数计算出需要展示的页码
#     total_count = models.PrettyNum.objects.filter(**data_dict).order_by("-level").count()
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
#             page_str_list.append('<li><a href="?page={}">{}</a></li>'.format(i, i))
#
#     # 将列表转换为字符串
#     page_string = mark_safe("".join(page_str_list))
#
#     # 根据字典内的内容在数据库中进行检索并根据level排序，[start: end]表示搜索范围即页数
#     # 此处使用-level即按照level从大到小进行检索
#     query_set = models.PrettyNum.objects.filter(**data_dict).order_by("-level")[start: end]
#
#     # 需要在html页面中进行替换的内容
#     context = {
#         "search_data": search_data,
#         "query_set": query_set,
#         "page_string": page_string,
#         "page_count": page_count,
#     }
#     return render(request, 'prettynum_list.html', context)

def prettynum_list(request):
    query_set = models.PrettyNum.objects.all()
    context = paginate_query_set(request, query_set, 'mobile')
    return render(request, 'prettynum_list.html', context)


# 添加靓号
def prettynum_add(request):
    if request.method == 'GET':
        form = PrettyNumModelForm()
        return render(request, 'prettynum_add.html', {'form': form})
    else:
        form = PrettyNumModelForm(data=request.POST)
        if form.is_valid():
            # 数据合法则保存至数据库
            form.save()
            return redirect('/prettynum/list/')
        else:
            return render(request, 'prettynum_add.html', {'form': form})


# 删除靓号
def prettynum_delete(request):
    nid = request.GET.get('nid')
    models.PrettyNum.objects.filter(id=nid).delete()
    return redirect('/prettynum/list/')


# 编辑靓号
def prettynum_edit(request, nid):
    row_object = models.PrettyNum.objects.filter(id=nid).first()

    if request.method == 'GET':
        # 根据id获取需要编辑的数据
        form = PrettyNumEditModelForm(instance=row_object)
        return render(request, 'prettynum_edit.html', {'form': form})
    else:
        form = PrettyNumEditModelForm(data=request.POST, instance=row_object)
        if form.is_valid():
            # 数据合法则保存至数据库
            form.save()
            return redirect('/prettynum/list/')
        else:
            return render(request, 'prettynum_edit.html', {'form': form})
