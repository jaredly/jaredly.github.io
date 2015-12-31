gen:
	hexo gen
	rsync -r public/ pages
