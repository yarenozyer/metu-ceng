package ceng.ceng351.foodrecdb;

import java.awt.*;
import java.sql.*;
import java.util.ArrayList;

public class FOODRECDB implements IFOODRECDB{

    private static String user = "e2448785"; // TODO: DONE
    private static String password = "q1mt7JghaRcpsqeJ"; //  TODO: DONE
    private static String host = "momcorp.ceng.metu.edu.tr"; // host name
    private static String database = "db2448785"; // TODO: DONE
    private static int port = 8080; // port

    private static Connection connection=null;

    @Override
    public void initialize() {
        String url = "jdbc:mysql://" + this.host + ":" + this.port + "/" + this.database;

        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            this.connection =  DriverManager.getConnection(url, this.user, this.password);
        }
        catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    @Override
    public int createTables() {

        int numberofTablesCreated = 0;

        //MenuItems(itemID:int, itemName:varchar(40), cuisine:varchar(20), price:int)
        String queryCreateMenuItemsTable = "create table MenuItems (" +
                "itemID int not null," +
                "itemName varchar(40)," +
                "cuisine varchar(20)," +
                "price int," +
                "primary key (itemID))";

        //Ingredients(ingredientID:int, ingredientName:varchar(40))
        String queryCreateIngredientsTable = "create table Ingredients (" +
                "ingredientID int not null," +
                "ingredientName varchar(40)," +
                "primary key (ingredientID))";

        //Includes(itemID:int, ingredientID:int)
        String queryCreateIncludesTable = "create table Includes (" +
                "itemID int not null," +
                "ingredientID int not null," +
                "primary key (itemID, ingredientID),"+
                "foreign key (itemID) references MenuItems(itemID),"+
                "foreign key (ingredientID) references Ingredients(ingredientID))";

        //Ratings(ratingID:int, itemID:int, rating:int, ratingDate:date)
        String queryCreateRatingsTable = "create table Ratings (" +
                "ratingID int not null," +
                "itemID int," +
                "rating int," +
                "ratingDate date," +
                "primary key (ratingID),"+
                "foreign key (itemID) references MenuItems(itemID))";

        //DietaryCategories(ingredientID:int, dietaryCategory:varchar(20)
        String queryCreateDietaryCategoriesTable = "create table DietaryCategories (" +
                "ingredientID int not null," +
                "dietaryCategory varchar(20)," +
                "primary key (ingredientID,dietaryCategory),"+
                "foreign key (ingredientID) references Ingredients(ingredientID))";


        try {
            Statement statement = this.connection.createStatement();

            //MenuItems Table
            statement.executeUpdate(queryCreateMenuItemsTable);
            numberofTablesCreated++;

            statement.executeUpdate(queryCreateIngredientsTable);
            numberofTablesCreated++;

            statement.executeUpdate(queryCreateIncludesTable);
            numberofTablesCreated++;

            statement.executeUpdate(queryCreateRatingsTable);
            numberofTablesCreated++;

            statement.executeUpdate(queryCreateDietaryCategoriesTable);
            numberofTablesCreated++;

            //close
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return numberofTablesCreated;
    }

    @Override
    public int dropTables() {
        int numberofTablesDropped = 0;

        //Player(number:integer, teamname:char(20), name:char(30), age:integer, position:char(3))
        String queryDropDietaryCategoriesTable = "drop table if exists DietaryCategories";
        String queryDropRatingsTable = "drop table if exists Ratings";
        String queryDropIncludesTable = "drop table if exists Includes";
        String queryDropIngredientsTable = "drop table if exists Ingredients";
        String queryDropMenuItemsTable = "drop table if exists MenuItems";


        try {
            Statement statement = this.connection.createStatement();

            statement.executeUpdate(queryDropDietaryCategoriesTable);
            numberofTablesDropped++;

            statement.executeUpdate(queryDropRatingsTable);
            numberofTablesDropped++;

            statement.executeUpdate(queryDropIncludesTable);
            numberofTablesDropped++;

            statement.executeUpdate(queryDropIngredientsTable);
            numberofTablesDropped++;

            statement.executeUpdate(queryDropMenuItemsTable);
            numberofTablesDropped++;

            //close
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberofTablesDropped;

    }

    @Override
    public int insertMenuItems(MenuItem[] items) {
        int numberOfRowsInserted = 0;
        for(int i=0; i<items.length;i++){

            //MenuItems(itemID:int, itemName:varchar(40), cuisine:varchar(20), price:int)
            try {
                MenuItem item = items[i];
                String query = "insert into MenuItems values ('" +
                        item.getItemID()+ "','" +
                        item.getItemName() + "','" +
                        item.getCuisine() + "','" +
                        item.getPrice() + "')";
                Statement statement = this.connection.createStatement();
                statement.executeUpdate(query);

                //Close
                statement.close();
                numberOfRowsInserted++;

            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return numberOfRowsInserted;
    }

    @Override
    public int insertIngredients(Ingredient[] ingredients) {
        int numberOfRowsInserted = 0;
        for(int i=0; i<ingredients.length;i++){

            //Ingredients(ingredientID:int, ingredientName:varchar(40))
            try {
                Ingredient ing = ingredients[i];
                String query = "insert into Ingredients values ('" +
                        ing.getIngredientID()+ "','" +
                        ing.getIngredientName() + "')";
                Statement statement = this.connection.createStatement();
                statement.executeUpdate(query);

                //Close
                statement.close();
                numberOfRowsInserted++;

            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return numberOfRowsInserted;
    }

    @Override
    public int insertIncludes(Includes[] includes) {
        int numberOfRowsInserted = 0;
        for(int i=0; i<includes.length;i++){

            //Includes(itemID:int, ingredientID:int)
            try {
                Includes inc = includes[i];
                String query = "insert into Includes values ('" +
                        inc.getItemID()+ "','" +
                        inc.getIngredientID() + "')";
                Statement statement = this.connection.createStatement();
                statement.executeUpdate(query);

                //Close
                statement.close();
                numberOfRowsInserted++;

            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return numberOfRowsInserted;
    }

    @Override
    public int insertDietaryCategories(DietaryCategory[] categories) {
        int numberOfRowsInserted = 0;
        for(int i=0; i<categories.length;i++){

            //DietaryCategories(ingredientID:int, dietaryCategory:varchar(20)
            try {
                DietaryCategory category = categories[i];
                String query = "insert into DietaryCategories values ('" +
                        category.getIngredientID()+ "','" +
                        category.getDietaryCategory() + "')";
                Statement statement = this.connection.createStatement();
                statement.executeUpdate(query);

                //Close
                statement.close();
                numberOfRowsInserted++;

            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return numberOfRowsInserted;
    }

    @Override
    public int insertRatings(Rating[] ratings) {
        int numberOfRowsInserted = 0;
        for(int i=0; i<ratings.length;i++){

            //Ratings(ratingID:int, itemID:int, rating:int, ratingDate:date)
            try {
                Rating rating = ratings[i];
                String query = "insert into Ratings values ('" +
                        rating.getRatingID()+ "','" +
                        rating.getItemID() + "','" +
                        rating.getRating() + "','" +
                        rating.getRatingDate() + "')";
                Statement statement = this.connection.createStatement();
                statement.executeUpdate(query);

                //Close
                statement.close();
                numberOfRowsInserted++;

            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return numberOfRowsInserted;
    }

    @Override
    public MenuItem[] getMenuItemsWithGivenIngredient(String name) {
        String query = "select distinct M.itemID, M.itemName, M.cuisine, M.price\n" +
                "from MenuItems M, Includes I, Ingredients ING\n" +
                "where M.itemID = I.itemID and I.ingredientID = ING.ingredientID and ING.ingredientName = ?\n"+
                "order by M.itemID asc;";

        try {
            PreparedStatement statement = this.connection.prepareStatement(query);
            statement.setString(1, name);
            ResultSet resSet = statement.executeQuery();
            ArrayList<MenuItem> results = new ArrayList<>();

            while (resSet.next()) {
                results.add(new MenuItem(
                        resSet.getInt("itemID"),
                        resSet.getString("itemName"),
                        resSet.getString("cuisine"),
                        resSet.getInt("price")
                ));
            }
            //Close
            statement.close();
            return results.toArray(new MenuItem[0]);

        } catch (SQLException e) {
            e.printStackTrace();
        }
        return new MenuItem[0];
    }

    @Override
    public MenuItem[] getMenuItemsWithoutAnyIngredient() {
        String query = "select distinct *\n" +
                "from MenuItems M\n" +
                "where NOT EXISTS(select M.itemID\n" +
                "from Includes I\n" +
                "where M.itemID = I.itemID)\n"+
                "order by M.itemID asc;";

        try {
            PreparedStatement statement = this.connection.prepareStatement(query);
            ResultSet resSet = statement.executeQuery();
            ArrayList<MenuItem> results = new ArrayList<>();

            while (resSet.next()) {
                results.add(new MenuItem(
                        resSet.getInt("itemID"),
                        resSet.getString("itemName"),
                        resSet.getString("cuisine"),
                        resSet.getInt("price")
                ));
            }
            //Close
            statement.close();
            return results.toArray(new MenuItem[0]);

        } catch (SQLException e) {
            e.printStackTrace();
        }
        return new MenuItem[0];
    }

    @Override
    public Ingredient[] getNotIncludedIngredients() {
        String query = "select distinct *\n" +
                "from Ingredients ING\n" +
                "where NOT EXISTS(select ING.ingredientID\n" +
                "from Includes I\n" +
                "where ING.ingredientID = I.ingredientID)\n"+
                "order by ING.ingredientID asc;";

        try {
            PreparedStatement statement = this.connection.prepareStatement(query);
            ResultSet resSet = statement.executeQuery();
            ArrayList<Ingredient> results = new ArrayList<>();

            while (resSet.next()) {
                results.add(new Ingredient(
                        resSet.getInt("ingredientID"),
                        resSet.getString("ingredientName")
                ));
            }
            //Close
            statement.close();
            return results.toArray(new Ingredient[0]);

        } catch (SQLException e) {
            e.printStackTrace();
        }
        return new Ingredient[0];
    }

    @Override
    public MenuItem getMenuItemWithMostIngredients() {
        String query = "select M.itemID, M.itemName, M.cuisine, M.price\n" +
                "from MenuItems M, Includes I\n" +
                "where M.itemID = I.itemID\n" +
                "group by M.itemID\n" +
                "having count(*) >= all (\n" +
                "select count(*) as count\n" +
                "from MenuItems M, Includes I\n" +
                "where M.itemID = I.itemID\n" +
                "group by M.itemID);";

        try {
            PreparedStatement statement = this.connection.prepareStatement(query);
            ResultSet resSet = statement.executeQuery();
            resSet.next();
            MenuItem result = new MenuItem(resSet.getInt("itemID"),
                    resSet.getString("itemName"),
                    resSet.getString("cuisine"),
                    resSet.getInt("price"));

            //Close
            statement.close();
            return result;

        } catch (SQLException e) {
            e.printStackTrace();
        }


        return null;
    }

    @Override
    public QueryResult.MenuItemAverageRatingResult[] getMenuItemsWithAvgRatings() {
        String query = "select distinct M.itemID, M.itemName, AVG(R.rating) as avgRating\n" +
                "from MenuItems M\n" +
                "left join Ratings R\n" +
                "on M.itemID = R.itemID and R.rating>0\n" +
                "group by M.itemID\n" +
                "order by avgRating desc;";

        try {
            PreparedStatement statement = this.connection.prepareStatement(query);
            ResultSet resSet = statement.executeQuery();
            ArrayList<QueryResult.MenuItemAverageRatingResult> results = new ArrayList<>();

            while (resSet.next()) {
                results.add(new QueryResult.MenuItemAverageRatingResult(
                        resSet.getString("itemID"),
                        resSet.getString("itemName"),
                        resSet.getString("avgRating")
                ));
            }
            //Close
            statement.close();
            return results.toArray(new QueryResult.MenuItemAverageRatingResult[0]);

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return new QueryResult.MenuItemAverageRatingResult[0];
    }

    @Override
    public MenuItem[] getMenuItemsForDietaryCategory(String category) {
        String query = "select distinct M.itemID, M.itemName, M.cuisine, M.price\n" +
                "from MenuItems M\n" +
                "where M.itemID not in (\n" +
                "select I.itemID\n" +
                "from Includes I, DietaryCategories D\n" +
                "where I.ingredientID = D.ingredientID and D.dietaryCategory <> ?\n)" +
                "order by M.itemID asc;";
                //the problem here is that I am taking all the items that has an ingredient that suits the
                //dietary restriction. all items should suit!!

        try {
            PreparedStatement statement = this.connection.prepareStatement(query);
            statement.setString(1, category);
            ResultSet resSet = statement.executeQuery();
            ArrayList<MenuItem> results = new ArrayList<>();

            while (resSet.next()) {
                results.add(new MenuItem(
                        resSet.getInt("itemID"),
                        resSet.getString("itemName"),
                        resSet.getString("cuisine"),
                        resSet.getInt("price")
                ));
            }
            //Close
            statement.close();
            return results.toArray(new MenuItem[0]);

        } catch (SQLException e) {
            e.printStackTrace();
        }
        return new MenuItem[0];
    }

    @Override
    public Ingredient getMostUsedIngredient() {
        String query = "select ING.ingredientID, ING.ingredientName\n" +
                "from Ingredients ING, Includes I\n" +
                "where ING.ingredientID= I.ingredientID\n" +
                "group by ING.ingredientID\n" +
                "having count(*) >= all(\n" +
                "select count(*) as count\n" +
                "from Includes I\n" +
                "group by I.ingredientID);";

        try {
            PreparedStatement statement = this.connection.prepareStatement(query);
            ResultSet resSet = statement.executeQuery();
            resSet.next();

            Ingredient result = new Ingredient(resSet.getInt("ingredientID"),
                    resSet.getString("ingredientName"));

            //Close
            statement.close();
            return result;

        } catch (SQLException e) {
            e.printStackTrace();
        }




        return null;
    }

    @Override
    public QueryResult.CuisineWithAverageResult[] getCuisinesWithAvgRating() {
        String query = "select M.cuisine, AVG(R.rating) as averageRating\n" +
                "from MenuItems M\n" +
                "left join Ratings R\n" +
                "on M.itemID = R.itemID and R.rating>0\n" +
                "group by M.cuisine\n"+
                "order by averageRating desc;";

        try {
            PreparedStatement statement = this.connection.prepareStatement(query);
            ResultSet resSet = statement.executeQuery();
            ArrayList<QueryResult.CuisineWithAverageResult> results = new ArrayList<>();

            while (resSet.next()) {
                results.add(new QueryResult.CuisineWithAverageResult(
                        resSet.getString("cuisine"),
                        resSet.getString("averageRating")
                ));
            }
            //Close
            statement.close();
            return results.toArray(new QueryResult.CuisineWithAverageResult[0]);

        } catch (SQLException e) {
            e.printStackTrace();
        }
        return new QueryResult.CuisineWithAverageResult[0];
    }

    @Override
    public QueryResult.CuisineWithAverageResult[] getCuisinesWithAvgIngredientCount() {
        String query = "select Temp.cuisine, AVG(Temp.count) as averageCount\n" +
                "from (select M.cuisine, COUNT(I.ingredientID) as count\n" +
                "from MenuItems M\n" +
                "left join Includes I\n" +
                "on M.itemID = I.itemID\n" +
                "group by M.itemID) as Temp\n" +
                "group by Temp.cuisine\n" +
                "order by averageCount desc;";


        try {
            PreparedStatement statement = this.connection.prepareStatement(query);
            ResultSet resSet = statement.executeQuery();
            ArrayList<QueryResult.CuisineWithAverageResult> results = new ArrayList<>();

            while (resSet.next()) {
                results.add(new QueryResult.CuisineWithAverageResult(
                        resSet.getString("cuisine"),
                        resSet.getString("averageCount")
                ));
            }
            //Close
            statement.close();
            return results.toArray(new QueryResult.CuisineWithAverageResult[0]);

        } catch (SQLException e) {
            e.printStackTrace();
        }
        return new QueryResult.CuisineWithAverageResult[0];
    }

    @Override
    public int increasePrice(String ingredientName, String increaseAmount) {
        String query = "update MenuItems\n" +
                "set price = price + ?\n" +
                "where itemID in (\n" +
                "select I.itemID\n" +
                "from Ingredients ING, Includes I\n" +
                "where I.ingredientID = ING.ingredientID and ING.ingredientName = ?);";

            //MenuItems(itemID:int, itemName:varchar(40), cuisine:varchar(20), price:int)
            try {

                PreparedStatement statement = this.connection.prepareStatement(query);
                statement.setString(1, increaseAmount);
                statement.setString(2, ingredientName);
                int result = statement.executeUpdate();

                //Close
                statement.close();
                return result;

            } catch (SQLException e) {
                e.printStackTrace();
            }

        return 0;
    }

    @Override
    public Rating[] deleteOlderRatings(String date) {
        ResultSet resSet;
        ArrayList<Rating> results = new ArrayList<>();
        String query = "select distinct *\n" +
                "from Ratings R\n" +
                "where R.ratingDate < ?" +
                "order by R.ratingID asc";



        //MenuItems(itemID:int, itemName:varchar(40), cuisine:varchar(20), price:int)
        try {

            PreparedStatement statement = this.connection.prepareStatement(query);
            statement.setString(1, date);
            resSet = statement.executeQuery();


            while(resSet.next()){
                results.add(new Rating(
                        resSet.getInt("ratingID"),
                        resSet.getInt("itemID"),
                        resSet.getInt("rating"),
                        resSet.getString("ratingDate")
                        ));


            }

            //close
            statement.close();


        } catch (SQLException e) {
            e.printStackTrace();
        }

        query = "delete from Ratings\n" +
                "where ratingDate > ?;";
        try {

            PreparedStatement statement = this.connection.prepareStatement(query);
            statement.setString(1, date);
            statement.executeUpdate();


            //close
            statement.close();


        } catch (SQLException e) {
            e.printStackTrace();
        }

        return results.toArray(new Rating[0]);
    }
}
