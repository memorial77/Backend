from django.shortcuts import render, redirect
from web import models
from web.utils.form import AdminModelForm, AdminEditModelForm, AdminResetModelForm


# 管理员列表
def admin_list(request):
    query_set = models.Admin.objects.all()

    context = {
        'query_set': query_set,
    }

    return render(request, 'admin_list.html', context)


def admin_add(request):
    title = "Add Admin"
    if request.method == 'GET':
        form = AdminModelForm()
        return render(request, 'change.html', {'title': title, 'form': form})
    else:
        form = AdminModelForm(data=request.POST)
        if form.is_valid():
            form.save()
            # 添加完成后重定向回部门列表页面查看添加结果
            return redirect('/admin/list')

    return render(request, 'change.html', {'title': title, 'form': form})


def admin_edit(request, nid):
    title = "Edit Admin"
    # 对象 or None
    row_object = models.Admin.objects.filter(id=nid).first()
    if not row_object:
        return redirect('admin/list')

    if request.method == "GET":
        form = AdminEditModelForm(instance=row_object)
        return render(request, 'change.html', {'title': title, 'form': form})
    else:
        form = AdminEditModelForm(data=request.POST, instance=row_object)
        if form.is_valid():
            form.save()
            return redirect('/admin/list/')
        else:
            return render(request, 'change.html', {'title': title, 'form': form})


def admin_delete(request, nid):
    models.Admin.objects.filter(id=nid).delete()
    return redirect('/admin/list/')


# 重置密码与编辑逻辑类似
def admin_reset(request, nid):
    # 对象 or None
    row_object = models.Admin.objects.filter(id=nid).first()
    if not row_object:
        return redirect('admin/list')

    title = "Reset Password - {}".format(row_object.username)

    if request.method == "GET":
        form = AdminResetModelForm()
        return render(request, 'change.html', {'title': title, 'form': form})
    else:
        form = AdminResetModelForm(data=request.POST, instance=row_object)
        if form.is_valid():
            form.save()
            return redirect('/admin/list/')
        else:
            return render(request, 'change.html', {'title': title, 'form': form})

