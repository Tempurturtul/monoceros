# Monoceros

- [Resources](#resources)
  - [Essential Git Commands](#essential-git-commands)
  - [NCURSES](#ncurses)  

## Resources

### Essential Git Commands

**Get this repository for the first time:**

```sh
git clone https://github.com/Tempurturtul/monoceros.git
```

**Update your local repository:**

```sh
git pull
```

**Check the state of your local repository:**

```sh
git status
```

**Switch to a new branch:**

```sh
git branch -b my-branch
```

**Switch to an existing branch:**

```sh
git checkout my-branch
```

**List branches:**

```sh
git branch --list
```

**Update a branch with changes from the master branch:**

```sh
# Switch to the branch to update
git checkout my-branch
# Pull in changes from the master branch (will fail if there are conflicts)
git pull origin master
```

**Commit (save) changes:**

```sh
# View the files that have changed
git status
# Stage changes to a file
git add my-file
# Commit all staged changes
git commit -m "My message describing what these changes do"
```

**Push changes to the remote repository:**

```sh
git push
```

### NCURSES

[tldp.org/HOWTO/NCURSES-Programming-HOWTO](http://www.tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html)

