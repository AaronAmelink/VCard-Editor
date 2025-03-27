import mysql.connector

class SQLManager:
    def __init__(self, name, user, password):
        self.dbName = name
        self.dbUser = user
        self.dbPass = password

        self.conn = None

        self.connect()

        self.conn.autocommit = True
        # prepare a cursor object using cursor() method
        cursor = self.conn.cursor()

        #Initialize the database

        createQuery="create table IF NOT EXISTS FILE (file_id int auto_increment PRIMARY KEY,  file_name varchar(60),  last_modified DATETIME, creation_time DATETIME NOT NULL)"
        res = cursor.execute(createQuery)
        print(res)

        createQuery="create table IF NOT EXISTS CONTACT (contact_id int auto_increment PRIMARY KEY,  name varchar(256) NOT NULL,  birthday DATETIME,  anniversary DATETIME,  file_id int, FOREIGN KEY (file_id) REFERENCES FILE(file_id) ON DELETE CASCADE)"
        res = cursor.execute(createQuery)
        print(res)

        cursor.close()
        self.conn.close()
        self.conn = None

    def close(self):
        # anything to close up shop?
        try:
            self.run_query("DROP TABLE CONTACT")
            self.run_query("DROP TABLE FILE")
        except:
            #gave it our best shot lol
            pass

    def connect(self):
        if (self.dbName == None or self.dbUser == None or self.dbPass == None):
            raise ValueError("Missing Database Information")
        if (self.conn == None):
            self.conn = mysql.connector.connect(host="dursley.socs.uoguelph.ca",database=self.dbName,user=self.dbName, password=self.dbPass)
            self.conn.autocommit = True
        return self.conn
    
    def run_query(self, query):
        res = []
        if (self.conn == None):
            self.connect()
        cursor = self.conn.cursor()
        
        cursor.execute(query)
        for r in cursor:
            res.append(r)
        
        cursor.close()
        self.conn.close()
        self.conn = None

        return res
    
    @staticmethod
    def get_date_for_sql(dt):
        if (dt == None):
            return "NULL"
        else:
            date = dt.split("T")[0]
            date = "00000000" + date
            
            formattedDate = list("0000-00-00")
            

            dateIndex = len(date) - 1 
            for i in range(len(formattedDate)-1, -1, -1):
                if (i not in [4, 7]):
                    formattedDate[i] = date[dateIndex]
                    dateIndex -= 1

            
            formattedTime = list("00:00:00")
            if (len(dt.split("T")) > 1):
                time = dt.split("T")[1]
                time = time.replace("Z", "")
                time = "000000" + time

                timeIndex = len(time) - 1
                for i in range(len(formattedTime)-1, -1, -1):
                    if (i not in [2, 5]):
                        formattedTime[i] = time[timeIndex]
                        timeIndex -= 1
            
            return "".join(formattedDate) + " " + "".join(formattedTime)
