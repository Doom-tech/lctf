#!/usr/bin/python3
from flask import Flask, make_response, request, render_template, render_template_string, redirect, abort
import db
import jwt


app = Flask(__name__)
private_key = open("keys/private.pem", "r").read()
public_key = open("keys/public.pem", "r").read()


@app.route('/')
def login_page():
	return render_template(f"login.html")


@app.route('/login', methods=["POST"])
def login():
	username, password = request.form["username"], request.form["password"]
	if user_id := db.login(username, password):
		resp = make_response(redirect('game'))
		resp.set_cookie("session", get_jwt(user_id, username))
		return resp
	else:
		return render_template(f"login.html", error=True)


@app.route('/register', methods=["GET", "POST"])
def register():
	if request.method == "POST":
		username, password = request.form["username"], request.form["password"]
		if db.register(username, password):
			resp = make_response(redirect('/'))
			return resp
		else:
			return render_template(f"register.html", error=True)
	else:
		return render_template(f"register.html")


@app.route('/logout', methods=["GET"])
def logout():
	resp = make_response(redirect('/'))
	resp.delete_cookie("session")
	return resp


@app.route('/game')
def game():
	try:
		session = jwt.decode(request.cookies['session'], public_key, algorithms=['RS256'])
		user_id, username = int(session["user_id"]), session["username"]
	except:
		return make_response(redirect('/logout'))
	score = db.get_user_score(user_id)
	return render_template(f"game.html", score=score, username=username)


@app.route('/update_score', methods=["POST"])
def update_score():
	try:
		session = jwt.decode(request.cookies['session'], public_key, algorithms=['RS256'])
		user_id = int(session["user_id"])
	except:
		abort(401)
	data = request.get_json()
	score = data["score"]
	old_score = db.get_user_score(user_id)
	db.update_user_score(user_id, score+old_score)
	return "", 200


@app.route('/scoreboard', methods=["GET"])
def get_scoreboard():
	try:
		session = jwt.decode(request.cookies['session'], public_key, algorithms=['RS256'])
		user_id = int(session["user_id"])
	except:
		abort(401)
	return render_template(f"scoreboard.html", score=db.get_scoreboard())


@app.route('/admin', methods=["GET"])
def admin():	
	return render_template(f"admin.html", users=db.get_users())

@app.route('/profile', methods=["GET"])
def get_profile():
	try:
		session = jwt.decode(request.cookies['session'], public_key, algorithms=['RS256'])
		user_id, username = int(session["user_id"]), session["username"]
	except:
		return make_response(redirect('/logout'))
	username, score, avatar, recovery = db.get_user_profile(user_id)
	return render_template(f"profile.html", username=username, score=score, avatar=avatar, recovery_code=recovery, user_id=user_id)


@app.route('/update_avatar', methods=['POST'])
def update_avatar():
	try:
		session = jwt.decode(request.cookies['session'], public_key, algorithms=['RS256'])
		user_id, username = int(session["user_id"]), session["username"]
	except:
		return make_response(redirect('/logout'))
	id, avatar = request.form["id"], request.form["avatar"]
	db.update_avatar(id, avatar)
	username, score, avatar, recovery = db.get_user_profile(id)
	return render_template(f"profile.html", username=username, score=score, avatar=avatar, recovery_code=recovery)


@app.route('/update_recovery', methods=["POST"])
def update_recovery_code():
	try:
		session = jwt.decode(request.cookies['session'], public_key, algorithms=['RS256'])
		user_id, username = int(session["user_id"]), session["username"]
	except:
		abort(401)
	recovery_code = request.form["recovery_code"]
	db.update_recovery_code(username, recovery_code)
	return make_response(redirect('/profile'))
		

def get_jwt(user_id, username):
	return jwt.encode({"user_id": user_id, "username": username}, private_key, algorithm="RS256")


if __name__ == '__main__':
	app.run(host="0.0.0.0")