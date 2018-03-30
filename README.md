# Secure Interactive Shell

## Usage

1. `make` builds a statically link binary `hw1`
2. add `xinetd` config

- the example `chroot` environment is `$HOME/tmproot`
- the binary is placed at `$HOME/tmproot/bin/hw1`

```shell
echo "\
service hw1
{
        type                    = UNLISTED
        flags                   = REUSE
        socket_type             = stream
        wait                    = no
        user                    = root
        server                  = /usr/sbin/chroot
        server_args             = $HOME/tmproot bin/hw1 1000 1000
        log_on_failure          += USERID
        disable                 = no
        port                    = 33445
}" | sudo tee /etc/xinetd.d/hw1
```

3. start `xinetd.service`

## help

```shell
cat {file}:              Display content of {file}.
cd {dir}:                Switch current working directory to {dir}.
chmod {mode} {file/dir}: Change the mode (permission) of a file or directory.
                         {mode} is an octal number.
                         Please do not follow symbolc links.
echo {str} [filename]:   Display {str}. If [filename] is given,
                         open [filename] and append {str} to the file.
exit:                    Leave the shell.
find [dir]:              List files/dirs in the current working directory
                         or [dir] if it is given.
                         Minimum outputs contatin file type, size, and name.
help:                    Display help message.
id:                      Show current euid and egid.
mkdir {dir}:             Create a new directory {dir}.
pwd:                     Print the current working directory.
rm {file}:               Remove a file.
rmdir {dir}:             Remove an empty directory.
stat {file/dir}:         Display detailed information of the given file/dir.
touch {file}:            Create {file} if it does not exist,
                         or update its access and modification timestamp.
umask {mode}:            Change the umask of the current session.
```
