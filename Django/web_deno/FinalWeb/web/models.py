from django.db import models


# Create your models here.

# 管理员
class Admin(models.Model):
    username = models.CharField(verbose_name="Username", max_length=32)
    password = models.CharField(verbose_name="Password", max_length=64)

    def __str__(self):
        return self.username


class Department(models.Model):
    """部门名称表"""
    title = models.CharField(verbose_name='部门名称', max_length=32)

    def __str__(self):
        return self.title


# Django 模型 Employee
class Employee(models.Model):
    """员工信息表"""
    name = models.CharField(verbose_name='NAME', max_length=16)
    password = models.CharField(verbose_name='PASSWORD', max_length=64)
    age = models.IntegerField(verbose_name='AGE')
    salary = models.DecimalField(verbose_name='SALARY', max_digits=10, decimal_places=2)
    # time = models.DateTimeField(verbose_name='TIME')
    time = models.DateField(verbose_name='TIME',default=None, blank=True, null=True)
    # 级联删除方式
    depart = models.ForeignKey(verbose_name='DEPARTMENT', to='Department', to_field='id', on_delete=models.CASCADE)
    # django中约束性别
    gender_choices = ((1, 'Male'), (2, 'Female'))
    gender = models.SmallIntegerField(verbose_name='GENDER', choices=gender_choices)

    def __str__(self):
        return self.name


# 靓号表
class PrettyNum(models.Model):
    mobile = models.CharField(verbose_name="Mobile", max_length=11)
    price = models.IntegerField(verbose_name="Price", default=0)

    level_choices = (
        (1, "Level 1"),
        (2, "Level 2"),
        (3, "Level 3"),
        (4, "Level 4"),
    )
    level = models.SmallIntegerField(verbose_name="Level", choices=level_choices, default=1)

    status_choices = (
        (1, "Occupied"),
        (2, "Unoccupied")
    )
    status = models.SmallIntegerField(verbose_name="Status", choices=status_choices, default=2)


class Contract(models.Model):
    """合同信息表"""

    employee = models.ForeignKey(verbose_name='Employee', to='Employee', to_field='id', on_delete=models.CASCADE)
    department = models.ForeignKey(verbose_name='Department', to='Department', to_field='id', on_delete=models.CASCADE)
    start_date = models.DateField(verbose_name='Start Date')
    end_date = models.DateField(verbose_name='End Date')
    # 这里用一个简单的文本字段来表示合同内容
    content = models.TextField(verbose_name='Content')

    # def __str__(self):
    #     return f"Contract for {self.employee.name} in {self.department.title}"
