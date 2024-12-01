import sqlite3
import bcrypt

def register(username, password):
	connection = sqlite3.connect('./database.db')
	cursor = connection.cursor()
	hashed_password = get_hashed_password(password.encode('utf-8'))
	cursor.execute('REPLACE INTO Users (username, password, score, avatar) VALUES (?, ?, ?, ?)', (username, hashed_password, 0, 1))
	connection.commit()
	connection.close()
	return True

def login(username, password):
	connection = sqlite3.connect('./database.db')
	cursor = connection.cursor()
	cursor.execute('SELECT id, password FROM Users WHERE username = ?', (username,))
	result = cursor.fetchone()
	if result:
		id = result[0]
		hashed_password = result[1]
	else:
		return False
	if not check_password(password.encode('utf-8'), hashed_password):
		return False
	connection.close()
	return id

def get_hashed_password(password):
	return bcrypt.hashpw(password, bcrypt.gensalt())

def check_password(password, hashed_password):
	return bcrypt.checkpw(password, hashed_password)

def update_recovery_code(username, recovery_code):
	connection = sqlite3.connect('./database.db')
	cursor = connection.cursor()
	cursor.execute('REPLACE INTO Recovery (recovery, user) VALUES (?, ?)', (recovery_code, username))
	connection.commit()
	connection.close()
	return True


def update_avatar(user_id, avatar):
	connection = sqlite3.connect('./database.db')
	cursor = connection.cursor()
	cursor.execute('UPDATE Users SET avatar = ? WHERE id = ?', (avatar, user_id))
	connection.commit()
	connection.close()
	return True

def get_user_score(user_id):
	connection = sqlite3.connect('./database.db')
	cursor = connection.cursor()
	cursor.execute('SELECT score FROM Users WHERE id = ?', (user_id,))
	result = cursor.fetchone()
	if result:
		score = result[0]
	connection.close()
	return score

def get_scoreboard():
	connection = sqlite3.connect('./database.db')
	cursor = connection.cursor()
	cursor.execute('SELECT username, score FROM Users WHERE score > 0 ORDER BY score DESC')
	return cursor.fetchall()

def get_users():
	connection = sqlite3.connect('./database.db')
	cursor = connection.cursor()
	cursor.execute('SELECT u.id, u.username, r.recovery FROM Users u LEFT JOIN Recovery r ON u.username = r.user ORDER BY u.id ASC')
	return cursor.fetchall()

def get_user_profile(user_id):
	connection = sqlite3.connect('./database.db')
	cursor = connection.cursor()
	cursor.execute('SELECT u.username, u.score, u.avatar, r.recovery FROM Users u LEFT JOIN Recovery r ON u.username = r.user WHERE u.id =?', (user_id,))
	result = cursor.fetchone()
	if result:
		username = result[0]
		score = result[1]
		avatar = result[2]
		recovery = result[3]
	connection.close()
	return username, score, avatar, recovery

def update_user_score(user_id, score):
	connection = sqlite3.connect('./database.db')
	cursor = connection.cursor()
	cursor.execute('UPDATE Users SET score = ? WHERE id = ?', (score, user_id))
	connection.commit()
	connection.close()