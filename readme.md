# PROCESSES TASK

Spring is also knocking at the door of the "Drink my friend" vineyard. As we all know, grapes love care and work, but most of all they love to work, which is usually rewarded with a good harvest. The vineyard needs a lot of helpers, so an application is being prepared for those who apply for the job.

Applications are invited from those who can work during the weeks of the spring season. The data is available in a file (see first homework) and any changes can be made.

"Drink my friend" vineyard (parent process) each morning will review the applications and needs for the day and start the worker process(es) for the day (child process(es))

Only a small bus is available that can carry a maximum of 5 people and there are two buses only. (The vineyard is a small company, so no more than 10 workers per day are needed.)

The vineyard starts the minibus(es), who, when ready to go, send a signal back to the parent, and then receive the name list of workers as a reply via pipe from the vineyard. This information is also written on the screen by the child process (bus).

As soon as the workers have been brought in, a summary message via message queue, is sent back to the vineyard, indicating how many of the requested workers have been brought in. This is written to the screen by the vineyard and then they terminate. The parent waits for the end of the child process and is ready to organise the next day's worker shuttle. (They can add or modify another applicant or start another day's bus run.)

Create a C (C++) program to do this, the solution should run either on opsys.inf.elte.hu server or on a Linux system similar to it. The solution must be presented to the tutor in the week following the deadline.

Upload the source files in .c or zip format here.

## Instructions
Run the following command in the linux terminal line:

```
chmod u+x main.c
gcc main.c -o vineyard && ./vineyard record.dat
gcc draft.c -o draft && ./draft record.txt

```

___
## Screenshots

1. Main(menu) screen
    <br>
    ![Main screen](screenshots/menu_screen.png)
    <br>
2. Insertion
    <br>
    ![Insertion of an applicant](screenshots/insert_first_applicant_alfredo_martins.png)
    <br>
    ![Insertion of another applicant](screenshots/insert_second_applicant_viktoria_bakonyi.png)
    <br>
3. Remove
    <br>
    ![Remove an applicant](screenshots/remove_nagy_gabor.png)
    <br>
    ![List applicants after remove Nagy Gábor](screenshots/list_after_remove_nagy_gabor.png)
    <br>
    ![List applicants by days after remove Nagy Gábor](screenshots/list_applicants_by_day_after_remove_nagy_gabor.png)
    <br>
4. List all applicants
    <br>
    ![List all the applicants](screenshots/list_all_apllicants.png)
    <br>
5. Modify
   <br>
    ![List applicants before modify](screenshots/all_applicants_by_days_before_modify_nikovits.png)
   <br>
    ![Modify an applicant](screenshots/modify_nikovits.png)
   <br>
    ![List applicants after modify](screenshots/all_applicants_by_days_after_modify_nikovits.png)
   <br>
    ![List applicants by days after modify](screenshots/all_applicants_by_days_before_modify_nikovits.png)
   <br>
6. Dismiss
    <br>
    ![Goodbye message](screenshots/dismiss_exit_the_program.png)
    <br>

> Made by MARTINS Alfredo.