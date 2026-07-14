#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winfsp.h>
#include <cygwin.h>

// Custom UNC handler to establish connection to SSHFS host
static NTSTATUS unc_handler(WINfspMountPoint* mount_point, LPCWSTR path, DWORD flags, void* context)
{
    // Parse the UNC path
    LPCWSTR host = NULL;
    LPCWSTR user = NULL;
    LPCWSTR password = NULL;
    LPCWSTR path = NULL;
    if (wcsstr(path, L"\\sshfs\\") == NULL)
        return STATUS_INVALID_PARAMETER;

    host = wcsstr(path, L"\\sshfs\\") + 8;
    user = wcsstr(host, L"@") + 1;
    password = wcsstr(user, L"!") + 1;
    path = wcsstr(password, L"\\") + 1;

    // Establish a connection to the SSHFS host
    cygwin_t *cygwin;
    cygwin = cygwin_init(NULL, NULL);
    if (cygwin == NULL)
        return STATUS_ACCESS_DENIED;

    // SSH connection setup
    ssh_session session;
    session = ssh_new();
    if (session == NULL)
        return STATUS_ACCESS_DENIED;

    // Authenticate with the SSH host
    ssh_userauth_password(session, user, password);
    if (ssh_userauth_password(session, user, password) != SSH_AUTH_SUCCESS)
        return STATUS_ACCESS_DENIED;

    // Open the SSHFS file system
    ssh_sftp* sftp_session;
    sftp_session = sftp_new(session);
    if (sftp_session == NULL)
        return STATUS_ACCESS_DENIED;

    // Create a new mount point
    WINfspMountPoint* new_mount_point;
    new_mount_point = WINfspMountPointCreate(mount_point->session, path, NULL, NULL);
    if (new_mount_point == NULL)
        return STATUS_ACCESS_DENIED;

    // Set the mount point's flags
    new_mount_point->flags |= WINFSP_MOUNTPOINT_FLAG_READ_ONLY;

    // Return the new mount point
    return STATUS_SUCCESS;
}

// Initialize the custom UNC handler
NTSTATUS init_unc_handler(WINfspSession* session)
{
    // Register the custom UNC handler
    WINfspMountPoint* mount_point;
    mount_point = WINfspMountPointCreate(session, NULL, NULL, unc_handler);
    if (mount_point == NULL)
        return STATUS_ACCESS_DENIED;

    // Return the new mount point
    return STATUS_SUCCESS;
}

// Entry point for the SSHFS-Win driver
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg_path)
{
    // Initialize the custom UNC handler
    WINfspSession* session;
    session = WINfspSessionCreate(driver, reg_path);
    if (session == NULL)
        return STATUS_ACCESS_DENIED;

    // Initialize the custom UNC handler
    init_unc_handler(session);

    // Return the driver's status
    return STATUS_SUCCESS;
}