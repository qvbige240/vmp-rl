/**
 * History:
 * ================================================================
 * 2019-08-16 qing.zou created
 *
 */

#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "vmp_log.h"
#include "vmp_process.h"

int kill_app(void)
{
    char cmd[64] = "killall -9 timapp";
    VMP_LOGW("%s", cmd);
    vpk_system_ex(cmd, 3);
    VMP_LOGI("kill timapp end.");

    return 0;
}

int vmp_run_process(const char *cmd, char *argv[])
{
    int ret = 0;
    pid_t pid = 0;
    return_val_if_fail(cmd, -1);

    if ((pid = fork()) < 0) {
        VMP_LOGE("fork failed.");
        //kill_all_process();
        kill_app();
        exit(-1);
    } else if (pid == 0) { /* child */
        VMP_LOGW("[daemon] child: %s (pid = %d) exec (ppid = %d)", cmd, getpid(), getppid());
        //if (execve(cmd, NULL, NULL) < 0) {
        argv[0] = (char *)cmd;
        if ((ret = execv(cmd, argv)) < 0)
        {
            VMP_LOGE("child execve failed!! ret = %d", ret);
            exit(ret);
        }
        VMP_LOGE("child execve failed!");
        usleep(100000);
    } else {
        VMP_LOGW("[daemon]father: %s (pid = %d) (fork pid = %d, ppid = %d)", argv[0], getpid(), pid, getppid());
        usleep(1000);

        //sleep(1);

        //int ret = kill(pid, 0);
        //VMP_LOGD("test kill 0 end, ret = %d", ret);

        //usleep(1000);
        //ret = kill(pid, SIGKILL);
        //VMP_LOGD("test kill end, ret = %d", ret);

        //sleep(1);
        //ret = kill(pid, 0);
        //VMP_LOGD("test kill 0  1end, ret = %d", ret);
    }

    return pid;
}

int vmp_waitpid(pid_t pid)
{
    int ret_pid = 0, cnt = 3;
    do
    {
        ret_pid = waitpid(pid, NULL, WNOHANG);
        if (ret_pid == 0)
        {
            VMP_LOGW("waitpid sleep");
            sleep(1);
        }
    } while (ret_pid == 0 && cnt-- > 0);

    if (ret_pid == 0)
        VMP_LOGW("progress not exited: %d, pid = %d", ret_pid, pid);
    else if (ret_pid == pid)
        VMP_LOGW("get child exit code: %d, pid = %d", ret_pid, pid);
    else {
        VMP_LOGE("process error, code = %d!\n", ret_pid);
        return -1;
    }

    return 0;
}
