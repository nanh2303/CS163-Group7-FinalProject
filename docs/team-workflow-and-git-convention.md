# Team Workflow and Git Convention

## 1. Purpose of this workflow

This workflow helps the team collaborate in a clear, consistent, and manageable way:

- Do not code directly on `main`
- Each member works on their own personal branch
- Completed work should be merged into `dev` through Pull Requests
- Code should build successfully before merging
- Commit messages must follow the agreed format

---

## 2. Main branches in the repository

### `main`
This is the most stable branch. It should be used for:
- the final submission version
- the demo version
- official releases

Rules:
- do not push directly unless necessary and agreed by the team
- only merge stable code from `dev`
- keep this branch clean and release-ready

### `dev`
This is the shared integration branch for the entire team.

Rules:
- all completed features should be merged into `dev`
- this branch is used to combine work from all members
- Pull Requests should target `dev`

### Personal branches
Each member has one personal development branch:

- `Anh`
- `Dung`
- `Long`
- `Khoa`

Rules:
- each member develops features on their own branch
- each member is responsible for keeping their branch organized
- after finishing a feature, create a Pull Request into `dev`

---

## 3. How this workflow works

Each member will work on their own branch instead of creating a new branch for every task.

For example:
- Anh works on branch `Anh`
- Dung works on branch `Dung`
- Long works on branch `Long`
- Khoa works on branch `Khoa`

After finishing a feature or a meaningful update:
1. commit the changes on the personal branch
2. push the branch to GitHub
3. create a Pull Request into `dev`

This workflow is simpler for the team because:
- each person always knows where they should work
- the branch structure is easy to understand
- `dev` remains the shared integration branch
- `main` remains the stable final branch

---

## 4. Standard workflow for each team member

### Step 1: switch to your personal branch
Example for Anh:
```bash
git checkout Anh
git pull origin Anh
```
### Step 2: develop your assigned feature
Write code, update files, and test your changes locally.

### Step 3: commit your changes using the correct format
Example:
```bash
git add .
git commit -m "feat: add hash table insertion"
```

### Step 4: push your branch to GitHub
Example:
```bash
git push origin Anh
```

### Step 5: create a Pull Request into dev
On GitHub:
base: dev
compare: your personal branch (Anh, Dung, Long, or Khoa)

### Step 6: wait for CI
GitHub Actions will automatically build the project.
If the build fails, continue fixing the code on your own branch and push again.

### Step 7: review and merge
When CI passes and the team agrees, merge the Pull Request into dev