from django.shortcuts import render
from django.utils.safestring import mark_safe
from web import models


def paginate_query_set(request, query_set, search_key):
    data_dict = {}
    search_data = request.GET.get('search', "")
    if search_data:
        data_dict[f"{search_key}__contains"] = search_data

    # 默认搜索的范围为第一页，page不传值或者首次访问该页面GET请求时默page认为1
    page = int(request.GET.get('page', 1))
    # 每页显示的数据条数
    page_size = 10
    start = (page - 1) * page_size
    end = page * page_size

    # 根据数据总条数计算出需要展示的页码
    total_count = query_set.filter(**data_dict).count()
    page_count, div = divmod(total_count, page_size)
    # 存在余数则总页码增加1
    if div:
        page_count += 1

    # 设置始终显示的页码数量
    displayed_pages = 4
    # 计算起始页码
    start_page = max(1, min(page_count - displayed_pages + 1, page - (displayed_pages // 2)))
    # 计算结束页码
    end_page = min(page_count, start_page + displayed_pages - 1)

    page_str_list = []
    for i in range(start_page, end_page + 1):
        if i == page:
            # 当前页码加上 active 类高亮
            page_str_list.append('<li class="active"><a href="?page={}">{}</a></li>'.format(i, i))
        else:
            if search_data:
                page_str_list.append('<li><a href="?page={}&search={}">{}</a></li>'.format(i, search_data, i))
            else:
                page_str_list.append('<li><a href="?page={}">{}</a></li>'.format(i, i))

    # 将列表转换为字符串
    page_string = mark_safe("".join(page_str_list))

    # 根据字典内的内容在数据库中进行检索并根据level排序，[start: end]表示搜索范围即页数
    query_set = query_set.filter(**data_dict)[start: end]

    # 需要渲染进HTML文件的数据
    context = {
        "search_data": search_data,
        "query_set": query_set,
        "page_string": page_string,
        "page_count": page_count,
    }
    return context

