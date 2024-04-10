from django.utils.deprecation import MiddlewareMixin
from django.shortcuts import redirect


class LoginMiddleware(MiddlewareMixin):

    @staticmethod
    def process_request(request):
        # 0.排除那些不需要登录就能访问的页面(如login界面)
        if request.path_info in ["/login/"]:
            return

        # 1.读取当前访问的用户的session信息，如果能读到，说明已登陆过，就可以继续向后走。
        info_dict = request.session.get("info")
        if info_dict:
            return

        # 2.直接访问其他页面但是没有登录过，重新回到登录页面
        return redirect('/login/')
