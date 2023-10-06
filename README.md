# ifj23

## GIT configuration
```bash
git config --global user.name "name"
git config --global user.email "email@domain.cz"
git config --global pull.rebase true
```

## Clonning of repository

```bash
git clone https://gitlab.com/en1qc/ifj23.git
```

## Adding of new changes
```bash
git checkout work
git pull
git branch <name of new branch>
git checkout <name of new branch>

# Making of changes (commits)

# creating commit
git add .
git commit -m "name of commit"

git push
```

## Testing and building

build without debug information
```bash
make build
```

build with debug information
```bash
make debug
```

run automatic tests
```bash
make test
```